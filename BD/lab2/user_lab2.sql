--10
create table KNP_TABLE (
    id number
);
create view KNP_VIEW as select * from KNP_TABLE;
drop table KNP_VIEW;
drop table KNP_TABLE;
--11
create table tableTask11 (
    id number,
    name varchar(10)
) tablespace KNP_QDATA;

insert into tableTask11 values (1, 'one');
insert into tableTask11 values (2, 'two');
insert into tableTask11 values (3, 'three');

select * from tableTask11;
drop table tableTask11;
