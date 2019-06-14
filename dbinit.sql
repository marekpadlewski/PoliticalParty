CREATE TABLE members
(id integer PRIMARY KEY,
password varchar(128) NOT NULL,
timestamp bigint NOT NULL,
isleader boolean NOT NULL);


CREATE TABLE projects
(id integer PRIMARY KEY,
authorityid integer NOT NULL);


CREATE TABLE actions
(id integer PRIMARY KEY,
projectid integer NOT NULL,
memberid integer NOT NULL,
authorityid integer NOT NULL,
type char(7) NOT NULL,
timestamp bigint NOT NULL,
upvotes integer DEFAULT 0 NOT NULL,
downvotes integer DEFAULT 0 NOT NULL,
FOREIGN KEY (projectid) REFERENCES projects(id),
FOREIGN KEY (memberid) REFERENCES members(id));


CREATE TABLE votes
(actionid integer NOT NULL,
memberid integer NOT NULL,
type char(8) NOT NULL,
FOREIGN KEY (memberid) REFERENCES members(id),
FOREIGN KEY (actionid) REFERENCES actions(id));


CREATE TABLE takenid
(id integer UNIQUE);


CREATE EXTENSION IF NOT EXISTS pgcrypto;

