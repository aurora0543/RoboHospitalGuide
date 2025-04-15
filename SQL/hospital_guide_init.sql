-- 创建数据库（如果不存在）
CREATE DATABASE IF NOT EXISTS HospitalGuide DEFAULT CHARACTER SET utf8mb4;
USE HospitalGuide;

-- 病人表
CREATE TABLE IF NOT EXISTS Patients (
    PatientID INT AUTO_INCREMENT PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    BirthDate DATE NOT NULL,
    Gender ENUM('Male', 'Female', 'Other') NOT NULL,
    VisitType ENUM('Outpatient', 'Inpatient') NOT NULL,
    PhotoPath VARCHAR(255)
);

CREATE TABLE IF NOT EXISTS Departments (
    DepartmentID INT AUTO_INCREMENT PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    X1 INT,
    Y1 INT,
    X2 INT,
    Y2 INT
);

CREATE TABLE IF NOT EXISTS Registrations (
    RegistrationID INT AUTO_INCREMENT PRIMARY KEY,
    PatientID INT NOT NULL,
    DepartmentID INT NOT NULL,
    AppointmentTime DATETIME NOT NULL,
    AdditionalNotes TEXT,
    FOREIGN KEY (PatientID) REFERENCES Patients(PatientID),
    FOREIGN KEY (DepartmentID) REFERENCES Departments(DepartmentID)
);

INSERT INTO Departments (Name, X1, Y1, X2, Y2) VALUES
('Diagnostic & Support', 0, 0, 14, 48),
('Registration', 20, 30, 80, 38),
('Emergency Room', 20, 38, 33, 48),
('Outpatient Department', 65, 38, 80, 48),
('Specialty Clinics', 20, 0, 33, 10),
('Mental Health & Rehab', 42, 0, 56, 10),
('Internal Medicine Departments', 20, 14, 33, 24),
('Restroom / Toilet', 65, 0, 79, 8),
('Surgical Departments', 42, 14, 56, 24),
('Obstetrics, Gynecology, Pediatrics', 56, 24, 80, 24),
('Patient Rooms', 85, 0, 98, 48);


-- INSERT INTO Patients (Name, BirthDate, Gender, VisitType, PhotoPath) VALUES
-- ('Alice Zhang', '1990-01-01', 'Female', 'Outpatient', '/photos/alice.jpg'),
-- ('Li Wei', '1988-06-12', 'Male', 'Inpatient', '/photos/liwei.jpg');