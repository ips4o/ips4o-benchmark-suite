library(tidyverse)
library(scales)

my_db <- src_postgres(user = 'axtman', password = '')

avgpar <- tbl(my_db, 'avgparallel')
datatypesizes <- tbl(my_db, 'datatypesizes')
avgpar <- avgpar %>% inner_join(datatypesizes)
avgpar <- avgpar %>% filter(size * dsize >= (2^21 * threads))
pfast <- tbl(my_db, 'pfast')

avgfast <- avgpar %>% filter(!(gen  %in% c("zero","sorted","reverse"))) %>% inner_join(pfast) %>% collect(n=Inf)
avgfast = avgfast %>% filter(algo  %in% c("ips4oparallel", "mcstlbq"))

avgparmin = avgfast %>% group_by(machine, gen, datatype, parallel, threads, size) %>% summarise(min = min(milli)) %>% collect(n=Inf)
num_instances = length(avgparmin$machine)

ratio_to_best = inner_join(avgparmin, avgfast) %>% mutate(ratio = milli/min) %>% collect(n = Inf)


performance = ratio_to_best %>% filter(ratio <= 1) %>% group_by(algo) %>% summarise(n = n() / num_instances) %>% collect(n=Inf) %>% add_column(ratio = 1) %>% collect(n=Inf)
step = 0.25
begin = 1
end = 3
for(r in seq(from = begin, to = end, by = step)) {
  r2 = r
  group_to_best = ratio_to_best %>% filter(ratio <= r2) %>% group_by(algo) %>% summarise(n = n() / num_instances) %>% collect(n=Inf) %>% add_column(ratio = r2) %>% collect(n=Inf)
  performance = union(performance, group_to_best)
}

performance1 = performance
ggplot(data = performance1, aes(x = ratio, y = n, group = algo)) +
  geom_line(aes(color=algo))+
  geom_point(aes(color=algo))

tb_name = "perfparips4oquickbal"
performance1 = performance1 %>% add_row(algo = 'ips4oparallel', n = 1, ratio = 4.5)
performance1 = performance1 %>% add_row(algo = 'ips2raparallel', n = 1, ratio = 4.5)
performance1 = performance1 %>% add_row(algo = 'regionsort', n = 1, ratio = 4.5)
performance1 = performance1 %>% add_row(algo = 'pbbssamplesort', n = 1, ratio = 4.5)
performance1 = performance1 %>% add_row(algo = 'mcstlbq', n = 1, ratio = 4.5)
formatted = performance1 %>% format_csv()
test = sub("\n", ") VALUES (", formatted)
test = gsub("\n", "),(", test)
test = gsub("\\((\\w+),", "\\('\\1',", test)
test = paste0(test, ";")
test = sub("),(;", ");", test, fixed=TRUE)
test = paste0("INSERT INTO ", tb_name, "(", test)

library(RPostgreSQL)
copyconn <- my_db$con
class(copyconn) <- "PostgreSQLConnection"

query <- paste0("drop table if exists ", tb_name, " CASCADE;")
sql <- query
dbSendQuery(copyconn, sql)

query <- paste0("create table ", tb_name, " (algo character varying(100), n real, ratio real);")
sql <- query
dbSendQuery(copyconn, sql)

query <- paste0( test)
sql <- query
dbSendQuery(copyconn, sql)

