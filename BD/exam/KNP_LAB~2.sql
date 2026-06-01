--create SEQUENCE seq;

create or replace procedure add_customer(
    p_cust_num out customers.cust_num%type,
    p_company customers.company%type,
    p_cust_rep customers.cust_rep%type,
    p_credit_limit customers.credit_limit%type
    )
is
    v_cust_num customers.cust_num%type;
    v_date date := sysdate;
    v_count number;
    v_added boolean := false;
begin
    select count(*) into v_count from customers where company = p_company;
    if v_count > 0  then
        RAISE_APPLICATION_ERROR(-20001,'Данное имя уже занято');
    end if;
    
    select seq.nextval into p_cust_num from dual;
    insert into customers values (p_cust_num,p_company,p_cust_rep,p_credit_limit);
    exception
        when others then 
        dbms_output.put_line('дата ' || v_date || ' ошибка ' || SQLERRM);
end;

set serveroutput on;
declare
    v_cust_num customers.cust_num%type;
begin
    add_customer(v_cust_num,'new',101,50000.00);
    dbms_output.put_line('код юзера ' || v_cust_num);
end;
select * from customers