drop table Details;
drop table Appointments;
drop table Dentests;
drop table Patients;
drop table Assistents;
drop table Procedures;


-- Entity Sets
CREATE TABLE Dentests (
    did VARCHAR(9) PRIMARY KEY,
    dname VARCHAR(50) NOT NULL,
    dphone VARCHAR(25) NOT NULL,
    daddress VARCHAR(50) NOT NULL
);

CREATE TABLE Patients (
    pid VARCHAR(9) PRIMARY KEY,
    pname VARCHAR(50) NOT NULL,
    pphone VARCHAR(25) NOT NULL,
    paddress VARCHAR(50) NOT NULL
);

CREATE TABLE Assistents (
    aid VARCHAR(9) PRIMARY KEY,
    aname VARCHAR(50) NOT NULL,
    aphone VARCHAR(25) NOT NULL,
    aaddress VARCHAR(50) NOT NULL,
    wage NUMBER NOT NULL
);

CREATE TABLE Procedures (
    proname VARCHAR(20) PRIMARY KEY,
    duration NUMBER(3,2) NOT NULL,
    cost DECIMAL(6,2) NOT NULL
);


-- Relationship Sets
CREATE TABLE Appointments (
    apid INTEGER PRIMARY KEY NOT NULL,
    did VARCHAR(9) REFERENCES Dentests NOT NULL,
    pid VARCHAR(9) REFERENCES Patients NOT NULL,
    aid VARCHAR(9) REFERENCES Assistents,
    aDate date NOT NULL,
    totalDuration number(3,2)
);

CREATE TABLE Details (
    apid INTEGER REFERENCES Appointments,
    proname VARCHAR(20) REFERENCES Procedures,
    occurences INTEGER NOT NULL,
    PRIMARY KEY (apid, proname)
);

INSERT INTO Procedures (proname, duration, cost)
VALUES('Cleaning', 1.5, 200);


INSERT INTO Procedures (proname, duration, cost)
VALUES('Crown', 3, 2000);


INSERT INTO Procedures (proname, duration, cost)
VALUES('Filling', 2, 500);


INSERT INTO Procedures (proname, duration, cost)
VALUES('Root canal', 2, 300);


INSERT INTO Procedures (proname, duration, cost)
VALUES('X-ray', 0.5, 100);
