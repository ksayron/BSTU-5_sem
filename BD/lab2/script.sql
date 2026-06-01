create table knp_t (x number primary key,s nvarchar2(50));
commit;
--2
insert into knp_t values (1,'Alina');
insert into knp_t values (2,'Kolya');
insert into knp_t values (3,'Sergei');
commit;
--3
update knp_t set s='Nikolay' where x=2;
commit;
--4
select * from knp_t where x>1;
--5
delete from knp_t where x=3;
commit;
--6
create table knp_t1 (id number primary key, x number, name nvarchar2(20),
constraint fk_x foreign key (x) references knp_t(x));
insert into knp_t1 values (1,1,'Minsk');
insert into knp_t1 values (2,2,'Brest');
commit;
--7
select t.x,t.s,t1.name from knp_t t inner join knp_t1 t1 on t.x=t1.x;
select t.x,t.s,t1.name from knp_t t left join knp_t1 t1 on t.x=t1.x;
select t.x,t.s,t1.name from knp_t t right join knp_t1 t1 on t.x=t1.x;

--8
drop table knp_t1;
drop table knp_t;