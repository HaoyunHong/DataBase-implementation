CREATE DATABASE CAFE;
USE CAFE;
CREATE TABLE menu(price INT, cost INT, kind CHAR, PRIMARY KEY(kind));
INSERT INTO menu(price, cost, kind) VALUES (15, 5, "milk coffee");
INSERT INTO menu(price, cost, kind) VALUES (20, 8, "milk cake");
INSERT INTO menu(price, cost, kind) VALUES (30, 12, "abc");
INSERT INTO menu(price, cost, kind) VALUES (15, 5, "red rose cake");
INSERT INTO menu(price, cost, kind) VALUES (22, 10, "pink rose cake");
INSERT INTO menu(price, cost, kind) VALUES (20, 8, "ki");
SELECT * from menu WHERE kind LIKE '%cake';
SELECT price from menu WHERE kind LIKE '%cake';
SELECT * from menu WHERE kind LIKE "milk%";
SELECT price from menu WHERE kind LIKE "milk%";
SELECT * from menu WHERE kind LIKE '%rose%';
SELECT price from menu WHERE kind LIKE '%rose%';
SELECT * from menu WHERE kind LIKE '_i';
SELECT price from menu WHERE kind LIKE 'k_';
DROP DATABASE CAFE;
 /*可以选择列输出*/
 /*支持单引号或者双引号*/