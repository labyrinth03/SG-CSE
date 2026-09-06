DROP DATABASE IF EXISTS project2;
CREATE DATABASE project2;
USE project2;

CREATE TABLE Customers (
    customer_ID INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    address VARCHAR(200),
    phone VARCHAR(20),
    gender ENUM('M', 'F') NOT NULL,
    annual_income DECIMAL(12,2)
);

CREATE TABLE Dealers (
    dealer_ID INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    location VARCHAR(100)
);

CREATE TABLE Brands (
    brand_ID INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL UNIQUE
);

CREATE TABLE Suppliers (
    supplier_ID INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL UNIQUE,
    contact_information VARCHAR(200)
);

CREATE TABLE Manufacturing_Plants (
    plant_ID INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    address VARCHAR(200)
);

CREATE TABLE Parts (
    part_ID INT PRIMARY KEY AUTO_INCREMENT,
    part_name VARCHAR(100) NOT NULL,
    part_type VARCHAR(50) NOT NULL,
    supplier_ID INT NOT NULL,
    plant_ID INT NOT NULL,
    manufactured_date DATE NOT NULL,
    FOREIGN KEY (supplier_ID)
        REFERENCES Suppliers(supplier_ID),
    FOREIGN KEY (plant_ID)
        REFERENCES Manufacturing_Plants(plant_ID)
);

CREATE TABLE Models (
    model_ID INT PRIMARY KEY AUTO_INCREMENT,
    brand_ID INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    year INT NOT NULL,
    base_price DECIMAL(12,2) NOT NULL,
    body_style VARCHAR(50) NOT NULL,
    FOREIGN KEY (brand_ID)
        REFERENCES Brands(brand_ID)
);

CREATE TABLE Vehicles (
    VIN VARCHAR(17) PRIMARY KEY,
    model_ID INT NOT NULL,
    plant_ID INT NOT NULL,
    manufacturing_date DATE NOT NULL,
    status ENUM('In Stock', 'Sold', 'Defective') NOT NULL,
    FOREIGN KEY (model_ID)
        REFERENCES Models(model_ID),
    FOREIGN KEY (plant_ID)
        REFERENCES Manufacturing_Plants(plant_ID)
);

CREATE TABLE Options (
    option_ID INT PRIMARY KEY AUTO_INCREMENT,
    color VARCHAR(50),
    engine_type VARCHAR(50),
    transmission_type VARCHAR(50)
);

CREATE TABLE Sales_Transactions (
    transaction_ID INT PRIMARY KEY AUTO_INCREMENT,
    customer_ID INT NOT NULL,
    dealer_ID INT NOT NULL,
    VIN VARCHAR(17) NOT NULL UNIQUE,
    date DATE NOT NULL,
    payment_method VARCHAR(50),
    sale_price DECIMAL(12,2) NOT NULL,
    FOREIGN KEY (customer_ID)
        REFERENCES Customers(customer_ID),
    FOREIGN KEY (dealer_ID)
        REFERENCES Dealers(dealer_ID),
    FOREIGN KEY (VIN)
        REFERENCES Vehicles(VIN)
);

CREATE TABLE Vehicle_Options (
    VIN VARCHAR(17) NOT NULL,
    option_ID INT NOT NULL,
    PRIMARY KEY (VIN, option_ID),
    FOREIGN KEY (VIN)
        REFERENCES Vehicles(VIN),
    FOREIGN KEY (option_ID)
        REFERENCES Options(option_ID)
);

CREATE TABLE Vehicle_Parts (
    VIN VARCHAR(17) NOT NULL,
    part_ID INT NOT NULL,
    PRIMARY KEY (VIN, part_ID),
    FOREIGN KEY (VIN)
        REFERENCES Vehicles(VIN),
    FOREIGN KEY (part_ID)
        REFERENCES Parts(part_ID)
);

CREATE TABLE Supplier_Model (
    supplier_ID INT NOT NULL,
    model_ID INT NOT NULL,
    PRIMARY KEY (supplier_ID, model_ID),
    FOREIGN KEY (supplier_ID)
        REFERENCES Suppliers(supplier_ID),
    FOREIGN KEY (model_ID)
        REFERENCES Models(model_ID)
);
