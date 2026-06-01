--1
--C:\Oracle\Soft\network\admin
--2
--sqlplus / as sysdba
--show parameter;
--3
--alter session set container=KNP_PDB;
--show con_name;
--alter pluggable database KNP_PDB open
--select tablespace_name,file_name from dba_data_files;
--select role from dba_roles;
--select username from dba_users;
--4
--regedit
--5
--net manager
--6
--sqlplus /nolog
--connect C##KNP/111@KNP_PDB
--7
grant 
    connect, 
    create session,
    restricted session, -- ????? ????? ??? 6 ????
    alter session, 
    create any table,
    insert any table,
    drop any table,
    SYSDBA
to C##KNP container = all;
 
show con_name;
alter system disable restricted session;
-- 10
select * from dba_segments where owner = 'C##KNP';

-- 11
create or replace view segment_summary as
select
    owner,
    segment_type,
    count(*) as segment_count,
    sum(extents) as total_extents,
    sum(blocks) as total_blocks,
    sum(bytes) / 1024 as total_size_kb
from
    dba_segments
group by
    owner,
    segment_type;
    
select * from segment_summary;