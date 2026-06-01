--1
create or replace procedure GET_TEACHERS(PCODE TEACHER.PULPIT%TYPE) is
begin
  for i in (select * from TEACHER where PULPIT = PCODE)
    loop
      dbms_output.put_line(i.TEACHER_NAME);
    end loop;
end;

begin
  GET_TEACHERS('????');
end;
--2
create or replace function GET_NUM_TEACHERS(PCODE TEACHER.PULPIT%TYPE) return number
  is
  num number;
begin
  select count(*) into num from TEACHER where PULPIT = PCODE;
  return num;
end;

begin
  dbms_output.put_line(GET_NUM_TEACHERS('????'));
end;

--3
create or replace procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE) is
begin
  for i in (select * from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE))
    loop
      dbms_output.put_line(i.TEACHER_NAME);
    end loop;
end;

begin
  GET_TEACHERS('????');
end;

create or replace procedure GET_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) is
begin
  for i in (select * from SUBJECT where PULPIT = PCODE)
    loop
      dbms_output.put_line(i.SUBJECT_NAME);
    end loop;
end;
begin
  GET_SUBJECTS('????');
end;
--4
create or replace procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE)
    is
    cursor GetTeachers is
        select TEACHER, TEACHER_NAME, P.PULPIT
        from TEACHER inner join PULPIT P on P.PULPIT = TEACHER.PULPIT
        where FACULTY = FCODE;

    m_teacher      TEACHER.TEACHER%TYPE;
    m_teacher_name TEACHER.TEACHER_NAME%TYPE;
    m_pulpit       TEACHER.PULPIT%TYPE;
begin
    open GetTeachers;
    fetch GetTeachers into m_teacher, m_teacher_name, m_pulpit;

    while (GetTeachers%found)
    loop
        DBMS_OUTPUT.PUT_LINE(m_teacher || ' ' || m_teacher_name || ' ' || m_pulpit);
        fetch GetTeachers into m_teacher, m_teacher_name, m_pulpit;
    end loop;

    close GetTeachers;
end GET_TEACHERS;

begin
    GET_TEACHERS('???');
end;

create or replace procedure GET_SUBJECTS (PCODE SUBJECT.PULPIT%TYPE)
is
    cursor GetSubjects is
    select * from SUBJECT where PULPIT=PCODE;

    m_subject SUBJECT.SUBJECT%type;
    m_subject_name SUBJECT.SUBJECT_NAME%type;
    m_pulpit SUBJECT.PULPIT%type;
begin
    open GetSubjects;
    fetch GetSubjects into m_subject, m_subject_name, m_pulpit;

    while (GetSubjects%found)
    loop
        DBMS_OUTPUT.PUT_LINE(m_subject || ' ' || m_subject_name || ' ' || m_pulpit);
        fetch GetSubjects into m_subject, m_subject_name, m_pulpit;
    end loop;
    close GetSubjects;

end GET_SUBJECTS;

begin
    GET_SUBJECTS('????');
end;

--5
create or replace function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%TYPE) return number
is
  num number;
begin
  select count(*) into num from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE);
  return num;
end;

begin
  dbms_output.put_line(GET_NUM_TEACHERS('???'));
end;

create or replace function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) return number
is
  num number;
begin
  select count(*) into num from SUBJECT where PULPIT = PCODE;
  return num;
end;

begin
  dbms_output.put_line(GET_NUM_SUBJECTS('????'));
end;

--6
create or replace package TEACHERS is
  procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE);
  procedure GET_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE);
  function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%TYPE) return number;
  function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) return number;
end TEACHERS;

create or replace package body TEACHERS is
  procedure GET_TEACHERS(FCODE FACULTY.FACULTY%TYPE) is
  begin
    for i in (select * from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE))
      loop
        dbms_output.put_line(i.TEACHER_NAME);
      end loop;
  end;

  procedure GET_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) is
  begin
    for i in (select * from SUBJECT where PULPIT = PCODE)
      loop
        dbms_output.put_line(i.SUBJECT_NAME);
      end loop;
  end;

  function GET_NUM_TEACHERS(FCODE FACULTY.FACULTY%TYPE) return number
    is
    num number;
  begin
    select count(*) into num from TEACHER where PULPIT in (select PULPIT from PULPIT where FACULTY = FCODE);
    return num;
  end;

  function GET_NUM_SUBJECTS(PCODE SUBJECT.PULPIT%TYPE) return number
    is
    num number;
  begin
    select count(*) into num from SUBJECT where PULPIT = PCODE;
    return num;
  end;
end TEACHERS;
--7
begin
dbms_output.put_line('(Task6)');
  TEACHERS.GET_TEACHERS('????');
  dbms_output.put_line('========');
  TEACHERS.GET_SUBJECTS('????');
  dbms_output.put_line('========');
  dbms_output.put_line(TEACHERS.GET_NUM_TEACHERS('???'));
  dbms_output.put_line('========');
  dbms_output.put_line(TEACHERS.GET_NUM_SUBJECTS('????'));
end;