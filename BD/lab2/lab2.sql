--1
create tablespace TS_KNP
datafile 'TS_KNP.dbf'
size 7m
autoextend on next 5m
maxsize 20m;

select TABLESPACE_NAME, STATUS, contents logging from SYS.DBA_TABLESPACES;
drop tablespace TS_KNP;
--2
create temporary tablespace TS_KNP_TEMP
    tempfile 'ts_KNP_TEMP.dbf'
    size 5M
    autoextend on next 3M
    maxsize 30M;
    
select TABLESPACE_NAME, STATUS, contents logging from SYS.DBA_TABLESPACES;
drop tablespace TS_KNP_TEMP;
--3
select * from SYS.DBA_TABLESPACES;
select * from SYS.DBA_DATA_FILES;
--4
alter session set "_ORACLE_SCRIPT" = true

create role RL_KNPCORE;
grant
    connect,
    create table,
    create view,
    create procedure,
    drop any table,
    drop any view,
    drop any procedure
to RL_KNPCORE;
drop role RL_KNPCORE;
--5
select * from DBA_ROLES where ROLE = 'RL_KNPCORE';
select * from DBA_SYS_PRIVS where GRANTEE = 'RL_KNPCORE';
--6
create profile PF_KNPCORE limit
    failed_login_attempts 7
    sessions_per_user 3 
    password_life_time 60 -- ????? ????? ??????
    password_reuse_time 365 -- ????? ?? ?????????? ????????????? ??????
    password_lock_time 1 -- ????? ?????????? ??????
    connect_time 180 -- ????? ???????????
    idle_time 30; --????? ???????
--7
select * from DBA_PROFILES;
select * from DBA_PROFILES where profile = 'PF_TDSCORE';
select * from DBA_PROFILES where profile = 'default';
--8
create user KNPCORE
    identified by 11111
    default tablespace TS_KNP
    temporary tablespace TS_KNP_TEMP
    profile PF_KNPCORE
    account unlock
    password expire;
    
grant
    create session,
    create table,
    create view,
    drop any table,
    drop any view
to KNPCORE;


drop user KNPCORE;

--11
create tablespace KNP_QDATA
    datafile 'KNP_QDATA1.dbf'
    size 10M
    offline;

select TABLESPACE_NAME, STATUS, contents logging from SYS.DBA_TABLESPACES;

alter tablespace KNP_QDATA online;

alter user KNPCORE quota 2M on KNP_QDATA;

drop tablespace KNP_QDATA including contents and datafiles ;


connect KNPCORE;
create table tableTask11 (
    id number,
    name varchar(10)
) tablespace KNP_QDATA;

insert into tableTask11 values (1, 'one');
insert into tableTask11 values (2, 'two');
insert into tableTask11 values (3, 'three');

select * from tableTask11;
drop table tableTask11;
