library(tidyverse)
library(scales)

my_db <- src_postgres(user = 'axtman', password = '')

avgfast <- tbl(my_db, 'avgsequential')
datatypesizes <- tbl(my_db, 'datatypesizes')
avgfast <- avgfast %>% inner_join(datatypesizes)
avgfast <- avgfast %>% filter(size * dsize >= (2^18 * threads))


avgfast <- avgfast %>% filter(!(gen  %in% c("zero","sorted","reverse")))
avgfast <- avgfast %>% filter(datatype  %in% c('uint32','uint64'))
#avgfast <- avgfast %>% filter(datatype  %in% c('double','uint32','uint64'))
avgfast = avgfast %>% filter(!(algo  %in% c('learnedsort','aspas','quickxsort','wikisort','ssss','yaroslavskiy','openjdkyaroslavskiy','ps4o','ips4oml','blockquicksort','stdsort','timsort')))

avgparmin = avgfast %>% group_by(machine, gen, datatype, parallel, threads, size) %>% summarise(min = min(milli))
avgparmin_coll = avgparmin %>% collect(n=Inf)
num_instances = length(avgparmin_coll$machine)

ratio_to_best = inner_join(avgparmin, avgfast) %>% mutate(ratio = milli/min) %>% collect(n = Inf)
test = ratio_to_best %>% filter(algo == 'ips4o') %>% collect(n=Inf)


performance = ratio_to_best %>% filter(ratio <= 1) %>% group_by(algo) %>% summarise(n = n() / num_instances) %>% collect(n=Inf) %>% add_column(ratio = 0) %>% collect(n=Inf)
step = 0.05
begin = 0.0
end = 2
for(r in seq(from = begin, to = end, by = step)) {
  r2 = r * r
  group_to_best = ratio_to_best %>% filter(ratio <= r2 + 1) %>% group_by(algo) %>% summarise(n = n() / num_instances) %>% collect(n=Inf) %>% add_column(ratio = r2) %>% collect(n=Inf)
  performance = union(performance, group_to_best)
}

step = 0.25
mybreaks=c()
for(r in seq(from = 0, to = end, by = step)) {
  r2 = r * r
  mybreaks <- c(mybreaks, round(r2, 2))
}

# Instead of scale_x_sqrt, we use our own transformation as scale_x_sqrt does not provide values for x < 0.
# The result is that it is not possible to add the x-break '0'. 
mysqrt_trans <- function() {
  trans_new("mysqrt", 
            transform = base::sqrt,
            inverse = function(x) ifelse(x<0, 0, x^2),
            domain = c(0, Inf))
}

performance1 = performance
ggplot(data = performance1, aes(x = ratio, y = n, group = algo)) +
  geom_line(aes(color=algo))+
  geom_point(aes(color=algo))+
  scale_x_continuous(trans="mysqrt", limits=c(0, NA), breaks=mybreaks)

tb_name = "perfseqprimitives"
performance1 = performance1 %>% add_row(algo = 'stdsort', n = -1, ratio = 4)
performance1 = performance1 %>% add_row(algo = 'timsort', n = -1, ratio = 4)
formatted = performance1 %>% mutate(ratio=sqrt(ratio)) %>% format_csv()
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