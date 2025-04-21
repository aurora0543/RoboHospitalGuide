# ************************************************************
# Sequel Ace SQL dump
# Version 20090
#
# https://sequel-ace.com/
# https://github.com/Sequel-Ace/Sequel-Ace
#
# Host: 192.168.1.104 (MySQL 5.5.5-10.11.11-MariaDB-0+deb12u1)
# Database: HospitalGuide
# Generation Time: 2025-04-18 10:02:30 +0000
# ************************************************************


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
SET NAMES utf8mb4;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE='NO_AUTO_VALUE_ON_ZERO', SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


# Dump of table Departments
# ------------------------------------------------------------

DROP TABLE IF EXISTS `Departments`;

CREATE TABLE `Departments` (
  `DepartmentID` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(100) NOT NULL,
  `X1` int(11) DEFAULT NULL,
  `Y1` int(11) DEFAULT NULL,
  `X2` int(11) DEFAULT NULL,
  `Y2` int(11) DEFAULT NULL,
  PRIMARY KEY (`DepartmentID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

LOCK TABLES `Departments` WRITE;
/*!40000 ALTER TABLE `Departments` DISABLE KEYS */;

INSERT INTO `Departments` (`DepartmentID`, `Name`, `X1`, `Y1`, `X2`, `Y2`)
VALUES
	(1,'Diagnostic & Support',0,0,14,48),
	(2,'Registration',20,30,80,38),
	(3,'Emergency Room',20,38,33,48),
	(4,'Outpatient Department',65,38,80,48),
	(5,'Specialty Clinics',20,0,33,10),
	(6,'Mental Health & Rehab',42,0,56,10),
	(7,'Internal Medicine Departments',20,14,33,24),
	(8,'Restroom / Toilet',65,0,79,8),
	(9,'Surgical Departments',42,14,56,24),
	(10,'Obstetrics, Gynecology, Pediatrics',56,24,80,24),
	(11,'Patient Rooms',85,0,98,48);

/*!40000 ALTER TABLE `Departments` ENABLE KEYS */;
UNLOCK TABLES;


# Dump of table Patients
# ------------------------------------------------------------

DROP TABLE IF EXISTS `Patients`;

CREATE TABLE `Patients` (
  `PatientID` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(100) NOT NULL,
  `BirthDate` date NOT NULL,
  `Gender` enum('Male','Female','Other') NOT NULL,
  `VisitType` enum('Outpatient','Inpatient') NOT NULL,
  `PhotoPath` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`PatientID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

LOCK TABLES `Patients` WRITE;
/*!40000 ALTER TABLE `Patients` DISABLE KEYS */;

INSERT INTO `Patients` (`PatientID`, `Name`, `BirthDate`, `Gender`, `VisitType`, `PhotoPath`)
VALUES
	(1,'Hua Li','2000-09-02','Male','Outpatient',NULL),
	(2,'Joshua French','1952-06-17','Female','Outpatient',NULL),
	(3,'David Delgado','1967-10-20','Male','Inpatient',NULL),
	(4,'Daniel Nixon','1935-09-15','Male','Inpatient',NULL),
	(5,'Timothy Lara','1983-10-13','Female','Inpatient',NULL),
	(6,'James Hamilton','1991-12-01','Male','Outpatient',NULL),
	(7,'Heather Duncan','1948-08-27','Female','Outpatient',NULL),
	(8,'Amy Floyd','2023-09-25','Male','Outpatient',NULL),
	(9,'Louis Peck','1973-06-17','Female','Inpatient',NULL),
	(10,'Kevin Spencer','2023-02-12','Female','Inpatient',NULL),
	(11,'Jason Jimenez','1995-10-07','Male','Inpatient',NULL),
	(12,'Lauren Mcdowell','1970-11-22','Male','Inpatient',NULL),
	(13,'Veronica Higgins','1961-01-28','Female','Inpatient',NULL),
	(14,'Erin Howard','1975-01-26','Male','Inpatient',NULL),
	(15,'Nicole Gonzalez','2023-06-16','Male','Inpatient',NULL),
	(16,'Brittany Estrada','2019-08-02','Male','Outpatient',NULL),
	(17,'Amber Hines','2000-08-25','Male','Inpatient',NULL),
	(18,'Julia Gaines','2003-03-27','Male','Outpatient',NULL),
	(19,'Melissa Daniels','1998-06-02','Male','Inpatient',NULL),
	(20,'Nancy Moore','1980-02-28','Male','Outpatient',NULL),
	(21,'Michael Horne','1952-11-17','Male','Inpatient',NULL),
	(22,'Jack Kerr','1973-04-01','Female','Inpatient',NULL),
	(23,'Kevin Mathis','1943-03-07','Female','Inpatient',NULL),
	(24,'Michael Harrison','2015-03-17','Female','Outpatient',NULL),
	(25,'Brittany Scott','2022-07-26','Female','Outpatient',NULL),
	(26,'Melinda Stewart','1941-11-19','Male','Inpatient',NULL),
	(27,'Jason Young IV','2023-08-31','Male','Inpatient',NULL),
	(28,'Lauren Chavez','1949-07-14','Female','Outpatient',NULL),
	(29,'Gina Cooper','1969-06-28','Female','Inpatient',NULL),
	(30,'Christopher Walton','1978-04-05','Female','Inpatient',NULL),
	(31,'Steven Patrick','1949-12-06','Female','Outpatient',NULL),
	(32,'Alison Wilkerson','1951-06-29','Male','Inpatient',NULL),
	(33,'Kayla Miller','1966-06-17','Male','Inpatient',NULL),
	(34,'James Perkins','1958-10-30','Male','Inpatient',NULL),
	(35,'Joshua Ali','1978-03-20','Female','Outpatient',NULL),
	(36,'Jose Anderson','1991-12-29','Male','Outpatient',NULL),
	(37,'Stephanie Webb','1997-07-16','Male','Inpatient',NULL),
	(38,'Marcia Rose','1967-09-10','Female','Outpatient',NULL),
	(39,'Paul Ramirez','1983-04-26','Female','Outpatient',NULL),
	(40,'Cristina Sanders','1981-11-15','Male','Inpatient',NULL),
	(41,'Glenn Martin','1947-03-31','Male','Outpatient',NULL),
	(42,'Tiffany Brown','1940-01-26','Male','Outpatient',NULL),
	(43,'Jessica Lee','1978-07-13','Female','Outpatient',NULL),
	(44,'Keith Myers','1957-08-13','Female','Inpatient',NULL),
	(45,'Alison Martin','1948-11-12','Female','Outpatient',NULL),
	(46,'Ryan Quinn','1928-11-25','Female','Inpatient',NULL),
	(47,'Kevin Collins','1961-08-27','Male','Outpatient',NULL),
	(48,'Anne Hunt','1939-11-18','Male','Inpatient',NULL),
	(49,'Lisa Jackson','1932-06-21','Male','Inpatient',NULL),
	(50,'Jorge Meyer','1959-04-01','Male','Inpatient',NULL),
	(51,'Christopher Jordan','1954-05-04','Male','Inpatient',NULL),
	(52,'Ronald Strickland','1943-07-24','Male','Inpatient',NULL),
	(53,'Dakota Burgess','2007-06-18','Female','Inpatient',NULL),
	(54,'Erin Reyes','2021-01-16','Female','Inpatient',NULL),
	(55,'Dennis Pearson','2011-08-16','Female','Outpatient',NULL),
	(56,'Gina Hampton','1955-05-12','Female','Inpatient',NULL),
	(57,'Cheryl Young','1958-02-07','Male','Inpatient',NULL),
	(58,'Mary Knight','1979-12-16','Male','Inpatient',NULL),
	(59,'Diana Lyons','2020-12-12','Female','Outpatient',NULL),
	(60,'Peter Pugh','1993-10-18','Male','Inpatient',NULL),
	(61,'Matthew Romero','2014-10-12','Female','Inpatient',NULL),
	(62,'Trevor Anderson','1992-03-17','Male','Outpatient',NULL),
	(63,'James Haney','2007-02-05','Male','Outpatient',NULL),
	(64,'Anthony Moore','2019-04-11','Male','Inpatient',NULL),
	(65,'Michael Huang','1998-07-23','Male','Inpatient',NULL),
	(66,'Michael Moore','1993-07-17','Female','Outpatient',NULL),
	(67,'Hannah Dixon','1956-09-23','Female','Outpatient',NULL),
	(68,'Carl Smith','1953-08-31','Male','Outpatient',NULL),
	(69,'Robert Barry','1948-05-30','Female','Outpatient',NULL),
	(70,'Albert Anderson','1958-03-18','Male','Inpatient',NULL),
	(71,'Victoria Cohen','1986-05-26','Male','Outpatient',NULL),
	(72,'Lisa Jackson','1982-08-15','Male','Inpatient',NULL),
	(73,'Natalie Jones','1985-10-30','Male','Outpatient',NULL),
	(74,'Cheyenne Roberts','1978-11-24','Female','Outpatient',NULL),
	(75,'Angela Hernandez','1952-03-06','Female','Inpatient',NULL),
	(76,'Eugene Rocha','1969-04-02','Female','Outpatient',NULL),
	(77,'Michael Johnson','1979-04-25','Male','Outpatient',NULL),
	(78,'Kathy Johnson','1929-06-09','Male','Inpatient',NULL),
	(79,'Robert Noble','1986-05-04','Male','Outpatient',NULL),
	(80,'William Meyer','1936-03-09','Female','Outpatient',NULL),
	(81,'Rachel Wright','2013-10-13','Female','Outpatient',NULL),
	(82,'Michelle Pena','1952-03-01','Female','Outpatient',NULL),
	(83,'Tina Hunter','2012-03-11','Female','Inpatient',NULL),
	(84,'Cathy Espinoza','2020-05-26','Female','Inpatient',NULL),
	(85,'Ashlee Clements','1934-10-24','Male','Outpatient',NULL),
	(86,'Stuart Shepherd','1974-01-21','Female','Outpatient',NULL),
	(87,'Alex Lyons','1938-06-28','Male','Inpatient',NULL),
	(88,'Stephanie Combs','1972-06-25','Female','Outpatient',NULL),
	(89,'Kevin Levy','1948-02-21','Male','Outpatient',NULL),
	(90,'Donna Johns','1982-04-25','Female','Outpatient',NULL),
	(91,'Alan Vang','2018-09-04','Female','Inpatient',NULL),
	(92,'Derek Kelley','1957-10-01','Female','Inpatient',NULL),
	(93,'Theresa Reynolds','1985-02-20','Female','Inpatient',NULL),
	(94,'Dr. Kristen Griffin','1925-08-21','Male','Inpatient',NULL),
	(95,'Nathaniel Warren','1925-10-24','Male','Outpatient',NULL),
	(96,'Jessica Eaton','1940-01-15','Female','Outpatient',NULL),
	(97,'Savannah Clark','1962-05-26','Male','Outpatient',NULL),
	(98,'David Duncan','1955-01-16','Male','Outpatient',NULL),
	(99,'Sarah Phillips','1995-06-27','Male','Inpatient',NULL),
	(100,'Laura Parker','1975-02-09','Male','Inpatient',NULL),
	(101,'Katie Kim','1985-07-14','Female','Outpatient',NULL);

/*!40000 ALTER TABLE `Patients` ENABLE KEYS */;
UNLOCK TABLES;


# Dump of table Registrations
# ------------------------------------------------------------

DROP TABLE IF EXISTS `Registrations`;

CREATE TABLE `Registrations` (
  `RegistrationID` int(11) NOT NULL AUTO_INCREMENT,
  `PatientID` int(11) NOT NULL,
  `DepartmentID` int(11) NOT NULL,
  `AppointmentTime` datetime NOT NULL,
  `AdditionalNotes` text DEFAULT NULL,
  PRIMARY KEY (`RegistrationID`),
  KEY `PatientID` (`PatientID`),
  KEY `DepartmentID` (`DepartmentID`),
  CONSTRAINT `Registrations_ibfk_1` FOREIGN KEY (`PatientID`) REFERENCES `Patients` (`PatientID`),
  CONSTRAINT `Registrations_ibfk_2` FOREIGN KEY (`DepartmentID`) REFERENCES `Departments` (`DepartmentID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

LOCK TABLES `Registrations` WRITE;
/*!40000 ALTER TABLE `Registrations` DISABLE KEYS */;

INSERT INTO `Registrations` (`RegistrationID`, `PatientID`, `DepartmentID`, `AppointmentTime`, `AdditionalNotes`)
VALUES
	(1,1,1,'2025-01-01 00:00:00',NULL),
	(2,1,7,'2025-04-21 17:30:00',NULL),
	(3,2,1,'2025-04-20 10:45:00',NULL),
	(4,3,5,'2025-04-23 17:45:00',NULL),
	(5,4,11,'2025-04-21 16:45:00',NULL),
	(6,5,10,'2025-04-17 13:00:00',NULL),
	(7,6,11,'2025-04-23 15:15:00',NULL),
	(8,7,3,'2025-04-19 08:15:00',NULL),
	(9,8,7,'2025-04-21 08:45:00',NULL),
	(10,9,4,'2025-04-18 16:15:00',NULL),
	(11,10,1,'2025-04-17 14:15:00',NULL),
	(12,11,5,'2025-04-17 13:00:00',NULL),
	(13,12,9,'2025-04-22 14:00:00',NULL),
	(14,13,5,'2025-04-20 10:15:00',NULL),
	(15,14,9,'2025-04-21 15:00:00',NULL),
	(16,15,11,'2025-04-21 17:15:00',NULL),
	(17,16,7,'2025-04-19 08:45:00',NULL),
	(18,17,9,'2025-04-17 13:45:00',NULL),
	(19,18,3,'2025-04-20 15:45:00',NULL),
	(20,19,8,'2025-04-17 16:15:00',NULL),
	(21,20,2,'2025-04-21 08:15:00',NULL),
	(22,21,4,'2025-04-18 12:15:00',NULL),
	(23,22,9,'2025-04-21 10:00:00',NULL),
	(24,23,8,'2025-04-19 16:15:00',NULL),
	(25,24,10,'2025-04-19 10:15:00',NULL),
	(26,25,2,'2025-04-22 12:30:00',NULL),
	(27,26,5,'2025-04-19 17:45:00',NULL),
	(28,27,6,'2025-04-18 09:45:00',NULL),
	(29,28,11,'2025-04-17 15:00:00',NULL),
	(30,29,11,'2025-04-18 12:00:00',NULL),
	(31,30,4,'2025-04-20 10:30:00',NULL),
	(32,31,10,'2025-04-20 08:00:00',NULL),
	(33,32,2,'2025-04-22 12:00:00',NULL),
	(34,33,1,'2025-04-21 15:15:00',NULL),
	(35,34,3,'2025-04-19 13:45:00',NULL),
	(36,35,1,'2025-04-17 17:00:00',NULL),
	(37,36,2,'2025-04-23 08:45:00',NULL),
	(38,37,10,'2025-04-17 10:15:00',NULL),
	(39,38,8,'2025-04-19 16:30:00',NULL),
	(40,39,3,'2025-04-23 15:30:00',NULL),
	(41,40,8,'2025-04-20 17:00:00',NULL),
	(42,41,5,'2025-04-17 14:30:00',NULL),
	(43,42,2,'2025-04-18 09:30:00',NULL),
	(44,43,9,'2025-04-19 10:15:00',NULL),
	(45,44,7,'2025-04-21 10:00:00',NULL),
	(46,45,11,'2025-04-22 12:00:00',NULL),
	(47,46,10,'2025-04-17 14:45:00',NULL),
	(48,47,5,'2025-04-18 08:45:00',NULL),
	(49,48,6,'2025-04-20 08:15:00',NULL),
	(50,49,1,'2025-04-21 10:00:00',NULL),
	(51,50,11,'2025-04-19 10:15:00',NULL),
	(52,51,6,'2025-04-23 15:30:00',NULL),
	(53,52,4,'2025-04-19 12:45:00',NULL),
	(54,53,3,'2025-04-18 09:30:00',NULL),
	(55,54,2,'2025-04-22 10:45:00',NULL),
	(56,55,9,'2025-04-20 08:30:00',NULL),
	(57,56,4,'2025-04-22 13:30:00',NULL),
	(58,57,2,'2025-04-18 11:30:00',NULL),
	(59,58,5,'2025-04-17 16:45:00',NULL),
	(60,59,7,'2025-04-22 16:00:00',NULL),
	(61,60,10,'2025-04-19 13:00:00',NULL),
	(62,61,9,'2025-04-18 11:15:00',NULL),
	(63,62,5,'2025-04-20 14:30:00',NULL),
	(64,63,4,'2025-04-19 08:00:00',NULL),
	(65,64,11,'2025-04-18 14:15:00',NULL),
	(66,65,3,'2025-04-17 09:15:00',NULL),
	(67,66,9,'2025-04-20 08:45:00',NULL),
	(68,67,2,'2025-04-21 10:45:00',NULL),
	(69,68,8,'2025-04-23 16:45:00',NULL),
	(70,69,7,'2025-04-18 17:30:00',NULL),
	(71,70,10,'2025-04-18 11:00:00',NULL),
	(72,71,11,'2025-04-22 12:30:00',NULL),
	(73,72,4,'2025-04-17 14:30:00',NULL),
	(74,73,7,'2025-04-23 08:45:00',NULL),
	(75,74,10,'2025-04-23 13:00:00',NULL),
	(76,75,8,'2025-04-21 13:15:00',NULL),
	(77,76,9,'2025-04-21 16:00:00',NULL),
	(78,77,8,'2025-04-21 14:15:00',NULL),
	(79,78,7,'2025-04-21 08:15:00',NULL),
	(80,79,2,'2025-04-18 08:30:00',NULL),
	(81,80,6,'2025-04-22 08:45:00',NULL),
	(82,81,8,'2025-04-18 12:30:00',NULL),
	(83,82,7,'2025-04-20 10:30:00',NULL),
	(84,83,3,'2025-04-17 15:00:00',NULL),
	(85,84,11,'2025-04-20 11:15:00',NULL),
	(86,85,5,'2025-04-23 10:15:00',NULL),
	(87,86,1,'2025-04-22 17:15:00',NULL),
	(88,87,9,'2025-04-21 10:45:00',NULL),
	(89,88,9,'2025-04-22 08:00:00',NULL),
	(90,89,6,'2025-04-22 15:30:00',NULL),
	(91,90,5,'2025-04-23 12:45:00',NULL),
	(92,91,5,'2025-04-19 15:30:00',NULL),
	(93,92,8,'2025-04-21 15:00:00',NULL),
	(94,93,3,'2025-04-20 08:45:00',NULL),
	(95,94,1,'2025-04-21 10:30:00',NULL),
	(96,95,8,'2025-04-22 09:15:00',NULL),
	(97,96,2,'2025-04-23 14:00:00',NULL),
	(98,97,8,'2025-04-20 09:30:00',NULL),
	(99,98,5,'2025-04-17 12:15:00',NULL),
	(100,99,9,'2025-04-20 12:00:00',NULL),
	(101,100,9,'2025-04-17 08:00:00',NULL),
	(102,101,8,'2025-04-22 17:00:00',NULL);

/*!40000 ALTER TABLE `Registrations` ENABLE KEYS */;
UNLOCK TABLES;



/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
