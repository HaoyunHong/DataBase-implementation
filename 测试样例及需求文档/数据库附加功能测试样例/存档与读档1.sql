CREATE DATABASE qaz;
CREATE DATABASE edc;
CREATE DATABASE ijn;
USE ijn;
CREATE TABLE nba(team INT, player INT, income DOUBLE, influence DOUBLE, PRIMARY KEY(team));
CREATE TABLE cba(team INT, player INT, income DOUBLE, influence DOUBLE, PRIMARY KEY(team));
CREATE TABLE wnba(team INT, player INT, income DOUBLE, influence DOUBLE, PRIMARY KEY(team));
INSERT INTO cba(team, player, income, influence) VALUES (2, 8, 700.0, 100.0);
INSERT INTO cba(team, player, income, influence) VALUES (1, 9, 720.0, 190.0);
INSERT INTO cba(team, player, income, influence) VALUES (9, 2, 990.0, 180.0);
INSERT INTO cba(team, player, income, influence) VALUES (1, 5, 788.0, 600.0);
INSERT INTO nba(team, player, income, influence) VALUES (100, 20, 7222.0, 60000.0);
INSERT INTO nba(team, player, income, influence) VALUES (80, 50, 7111.0, 61293.0);
INSERT INTO wnba(team, player, income, influence) VALUES (7, 4, 9.3, 6.0);
SAVE TABLE cba FROM DATABASE ijn;
SHOW DATABASES;
CREATE DATABASE acs;
SHOW DATABASES;

