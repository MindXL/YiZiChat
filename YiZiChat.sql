DROP DATABASE IF EXISTS `YiZiChat`;

CREATE DATABASE `YiZiChat`
DEFAULT CHARACTER SET=ucs2
COLLATE=ucs2_general_ci;

USE `YiZiChat`;

CREATE TABLE `User` (
    `id` INT PRIMARY KEY AUTO_INCREMENT,
    `phone` CHAR(11) NOT NULL UNIQUE,
    `password` VARCHAR(20) NOT NULL CHECK(char_length(`password`) >= 8),
    `nickname` VARCHAR(20) UNIQUE NOT NULL CHECK(length(`nickname`) >= 2),
    `join_time` TIMESTAMP NOT NULL,
    `is_admin` TINYINT NOT NULL DEFAULT 0 CHECK(`is_admin` IN (0, 1))
);

CREATE TABLE `Channel` (
    `id` INT PRIMARY KEY AUTO_INCREMENT,
    `name` VARCHAR(20) NOT NULL,
    `join_time` TIMESTAMP NOT NULL,
    `description` VARCHAR(500)
);

CREATE TABLE `Transcript` (
    `cid` INT NOT NULL,
    `uid` INT NOT NULL,
    `time` TIMESTAMP NOT NULL,
    `content` VARCHAR(500) NOT NULL,
    FOREIGN KEY(`cid`) REFERENCES `Channel`(`id`),
    FOREIGN KEY(`uid`) REFERENCES `User`(`id`)
);

CREATE TABLE `Mute` (
    `id` INT PRIMARY KEY,
    `start` TIMESTAMP NOT NULL,
    `end` TIMESTAMP NOT NULL,
    FOREIGN KEY(`id`) REFERENCES `User`(`id`),
    CHECK(`start` < `end`)
);

CREATE TABLE `Ban` (
    `id` INT PRIMARY KEY,
    `start` TIMESTAMP NOT NULL,
    `end` TIMESTAMP NOT NULL,
    FOREIGN KEY(`id`) REFERENCES `User`(`id`),
    CHECK(`start` < `end`)
);

-- unix_timestamp(now())
INSERT INTO `User`(`phone`,`password`,`nickname`,`join_time`,`is_admin`) VALUES
('13312345678','123456789','Mind',now(),1),
('14412345678','123456789','user144',now(),0),
('15598765432','987654321','user155',now(),0),
('16648918761','1q2w3e4r5t_-','一二三四',now(),1);

INSERT INTO `Channel`(`name`,`join_time`,`description`) VALUES
("频道1",now(),"频道1的描述"),
("频道2",now(),"频道2的描述");
