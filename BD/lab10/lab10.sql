
alter pluggable database KNP_PDB open;

   
ALTER TABLE TEACHER
ADD (BIRTHDAY DATE, SALARY NUMBER);

UPDATE TEACHER
SET BIRTHDAY = TRUNC(SYSDATE) - FLOOR(DBMS_RANDOM.VALUE(365*23, 365*3)),
    SALARY = FLOOR(DBMS_RANDOM.VALUE(30000, 80000));


ALTER TABLE TEACHER
DROP COLUMN BIRTHDAY;

ALTER TABLE TEACHER
DROP COLUMN SALARY;

  
select * from TEACHER;

--2
select regexp_substr(teacher_name, '(\S+)', 1, 1) || ' ' ||
       substr(regexp_substr(teacher_name, '(\S+)', 1, 2), 1, 1) || '. ' ||
       substr(regexp_substr(teacher_name, '(\S+)', 1, 3), 1, 1) || '. ' as FIO
from teacher;

--3
select * from teacher where TO_CHAR(birthday, 'd') = 2;

--4
--drop view next_month_birthday

create or replace view next_month_birthday as
select * from teacher where TO_CHAR(birthday, 'mm') = (select TO_CHAR(last_day(sysdate) + 1, 'mm') from dual);

select * from next_month_birthday;

--5-drop view teacher_number_a_month

create or replace view teacher_number_a_month as
select TO_CHAR(birthday, 'Month') mon, count(*) num
    from teacher group by TO_CHAR(birthday, 'Month')
    order by num desc;
    
select * from teacher_number_a_month;

--6
declare
    cursor teacher_annyversary
        return teacher%rowtype is
        select *
        from teacher
        where MOD((TO_CHAR(sysdate, 'yyyy') - TO_CHAR(birthday, 'yyyy') + 1), 10) = 0;
    v_teacher  TEACHER%rowtype;
begin
    open teacher_annyversary;

    fetch teacher_annyversary into v_teacher;

    while (teacher_annyversary%found)
        loop
            dbms_output.put_line(v_teacher.teacher || ' ' || v_teacher.teacher_name || ' ' || v_teacher.pulpit || ' ' ||
                                 v_teacher.birthday || ' ' || v_teacher.salary);
            fetch teacher_annyversary into v_teacher;
        end loop;

    close teacher_annyversary;
end;


--7
declare
    cursor teachers_avg_salary is
        select pulpit, floor(avg(salary)) as AVG_SALARY
        from TEACHER
        group by pulpit;
    cursor faculty_avg_salary is
        select FACULTY, AVG(SALARY)
        from TEACHER
                 join PULPIT P on TEACHER.PULPIT = P.PULPIT
        group by FACULTY;
    cursor faculties_avg_salary is
        select AVG(SALARY)
        from TEACHER;
        
    m_pulpit  TEACHER.PULPIT%type;
    m_salary  TEACHER.SALARY%type;
    m_faculty PULPIT.FACULTY%type;
begin

    dbms_output.put_line('Pulpuits:');
    open teachers_avg_salary;
    fetch teachers_avg_salary into m_pulpit, m_salary;

    while (teachers_avg_salary%found)
        loop
            dbms_output.put_line(m_pulpit || ' ' || m_salary);
            fetch teachers_avg_salary into m_pulpit, m_salary;
        end loop;
    close teachers_avg_salary;

    dbms_output.put_line('');
    dbms_output.put_line('Faculties:');
    open faculty_avg_salary;
    fetch faculty_avg_salary into m_faculty, m_salary;

    while (faculty_avg_salary%found)
        loop
            dbms_output.put_line(m_faculty || ' ' || m_salary);
            fetch faculty_avg_salary into m_faculty, m_salary;
        end loop;
    close faculty_avg_salary;

    dbms_output.put_line('');
    dbms_output.put_line('All facultes:');
    open faculties_avg_salary;
    fetch faculties_avg_salary into m_salary;
    dbms_output.put_line(round(m_salary, 2));
    close faculties_avg_salary;
end;

--8
DECLARE
    TYPE location_record IS RECORD (
        street   VARCHAR2(100),
        city     VARCHAR2(50)
    );

    TYPE Emp_with_address_record IS RECORD (
        emp_id   NUMBER(6),
        first_name VARCHAR2(50),
        last_name  VARCHAR2(50),
        address    location_record
    );

    emp_with_address Emp_with_address_record;
    emp_address location_record;
BEGIN
    emp_with_address.emp_id := 203;
    emp_with_address.first_name := 'Nikolay';
    emp_with_address.last_name := 'Kucheruk';
    
    emp_address.street := 'Belorusskaya';  
    emp_address.city := 'Minsk';  
    emp_with_address.address := emp_address;

    DBMS_OUTPUT.PUT_LINE('Employee ID: ' || emp_with_address.emp_id);
    DBMS_OUTPUT.PUT_LINE('Name: ' || emp_with_address.first_name || ' ' || emp_with_address.last_name);
    DBMS_OUTPUT.PUT_LINE('Address: ' || emp_with_address.address.street || ', ' || 
                         emp_with_address.address.city);
END;
    











