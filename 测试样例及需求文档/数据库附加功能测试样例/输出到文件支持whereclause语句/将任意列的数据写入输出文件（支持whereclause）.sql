CREATE DATABASE OOP;
USE OOP;
CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "a");
INSERT INTO poi(mnb, bvc, dfj) VALUES (7, 7, "a");
DELETE FROM poi WHERE lkj>3 OR mnb=7 AND lkj=10;
SELECT * from poi;
SELECT * INTO OUTFILE 'output_file' FROM poi WHERE bvc>4;
SELECT bvc INTO OUTFILE 'outout_file2' FROM poi WHERE bvc<7;
DROP DATABASE OOP;