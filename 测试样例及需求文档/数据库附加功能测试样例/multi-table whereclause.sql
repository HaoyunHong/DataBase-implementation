CREATE DATABASE OOP_is_so_difficult;
USE OOP_is_so_difficult;
CREATE TABLE life1(grade INT, PRIMARY KEY(grade), girls CHAR);
CREATE TABLE life2(gpa DOUBLE, prize INT, eat INT, PRIMARY KEY(gpa));
INSERT INTO life1(grade, girls) VALUES (95, "a");
INSERT INTO life1(grade, girls) VALUES (100, "b");
INSERT INTO life1(grade, girls) VALUES (88, "a");
INSERT INTO life1(grade, girls) VALUES (90, "a");
INSERT INTO life1(grade, girls) VALUES (0, "c");
INSERT INTO life2(gpa, prize, eat) VALUES (4.0, 20000, 100);
INSERT INTO life2(gpa, prize, eat) VALUES (3.99, 19999, 99);
INSERT INTO life2(gpa, prize, eat) VALUES (3.88, 15000, 200);
INSERT INTO life2(gpa, prize, eat) VALUES (1.0, 1000000, 500);
SELECT * FROM life1;
SELECT life1.girls, life2.eat FROM life1, life2 WHERE life1.grade<life2.eat AND life1.girls="a";
SELECT * FROM life1, life2 WHERE life1.grade=life2.eat AND life1.girls="b";

