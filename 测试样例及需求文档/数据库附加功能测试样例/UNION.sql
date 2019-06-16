CREATE DATABASE OOP_is_so_difficult;
USE OOP_is_so_difficult;
CREATE TABLE class_liu(score INT, gpa DOUBLE, name1 CHAR, PRIMARY KEY(name1));
CREATE TABLE class_huang(score INT, gpa DOUBLE, name2 CHAR, PRIMARY KEY(name2));
INSERT INTO class_liu(score, gpa, name1) VALUES (100, 4.0, "a");
INSERT INTO class_liu(score, gpa, name1) VALUES (95, 4.0, "b");
INSERT INTO class_liu(score, gpa, name1) VALUES (89, 3.6, "c");
INSERT INTO class_liu(score, gpa, name1) VALUES (84, 3.3, "d");
INSERT INTO class_huang(score, gpa, name2) VALUES (100, 4.0, "A");
INSERT INTO class_huang(score, gpa, name2) VALUES (90, 4.0, "B");
INSERT INTO class_huang(score, gpa, name2) VALUES (88, 3.6, "C");
INSERT INTO class_huang(score, gpa, name2) VALUES (83, 3.3, "D");
SELECT gpa, name1 from class_liu UNION gpa, name2 from class_huang ORDER BY gpa;
SELECT gpa, name2 from class_huang UNION gpa, name1 from class_liu ORDER BY gpa;
SELECT gpa, name1 from class_liu UNION ALL gpa, name2 from class_huang ORDER BY gpa;
SELECT gpa, name2 from class_huang UNION ALL gpa, name1 from class_liu ORDER BY gpa;
SELECT score, name1 from class_liu UNION score, name2 from class_huang ORDER BY score;
SELECT score, name2 from class_huang UNION score, name1 from class_liu ORDER BY score;