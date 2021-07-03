SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- remove old tables
-- -----------------------------------------------------
drop table if exists bgdb.synapseconnections;
drop table if exists bgdb.synapses; 
drop table if exists bgdb.neurons; 
drop table if exists bgdb.user;
-- -----------------------------------------------------
-- Schema bgdb
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema bgdb
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `bgdb` DEFAULT CHARACTER SET utf8 ;
USE `bgdb` ;

-- -----------------------------------------------------
-- Table `bgdb`.`equation`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bgdb`.`equation` ;

CREATE TABLE IF NOT EXISTS `bgdb`.`equation` (
  `equationId` BIGINT NOT NULL AUTO_INCREMENT,
  `equationText` VARCHAR(250) NULL DEFAULT NULL,
  PRIMARY KEY (`equationId`))
ENGINE = InnoDB;

-- -----------------------------------------------------
-- Table `bgdb`.`neuron`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bgdb`.`neuron` ;

CREATE TABLE IF NOT EXISTS `bgdb`.`neuron` (
  `neuronId` BIGINT NOT NULL AUTO_INCREMENT,
  `xCoord` INT NULL DEFAULT NULL,
  `yCoord` INT NULL DEFAULT NULL,
  `zCoord` INT NULL DEFAULT NULL,
  `assignedNode` VARCHAR(45) NULL,
  `equationId` bigINT NULL DEFAULT NULL,
  region varchar(40) null,
  PRIMARY KEY (`neuronId`),
  CONSTRAINT `neuron_fk_equationid`
    FOREIGN KEY (equationId)
    REFERENCES `bgdb`.`equation` (equationId)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bgdb`.`synapse`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bgdb`.`synapse` ;

CREATE TABLE IF NOT EXISTS `bgdb`.`synapse` (
  `synapseId` BIGINT NOT NULL AUTO_INCREMENT,
  `xCoord` INT NULL DEFAULT NULL,
  `yCoord` INT NULL DEFAULT NULL,
  `zCoord` INT NULL DEFAULT NULL,
  `neuronId` BIGINT NULL DEFAULT NULL,
  `equationId` bigINT NULL DEFAULT NULL,
  PRIMARY KEY (`synapseId`),
  CONSTRAINT `synapse_fk_equationid`
    FOREIGN KEY (equationId)
    REFERENCES `bgdb`.`equation` (equationId)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `synapse_fk_neuronid`
    FOREIGN KEY (neuronId)
    REFERENCES `bgdb`.`neuron` (neuronId)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `bgdb`.`user`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bgdb`.`user` ;

CREATE TABLE IF NOT EXISTS `bgdb`.`user` (
  `userId` BIGINT NOT NULL AUTO_INCREMENT,
  `userName` VARCHAR(45) NOT NULL,
  `passwordHash` VARCHAR(50) NOT NULL,
  `salt` VARCHAR(6) NOT NULL,
  `firstName` VARCHAR(45) NOT NULL,
  `lastName` VARCHAR(45) NOT NULL,
  `notes` VARCHAR(200) NOT NULL,
  `accountEnabled` CHAR(1) NOT NULL DEFAULT 'n',
  `accountExpirationDate` DATETIME NULL DEFAULT NULL,
  `passwordExpirationDate` DATETIME NULL DEFAULT NULL,
  `permissionLevel` INT NOT NULL, 
  UNIQUE INDEX `userName` (`userName` ASC) VISIBLE,
  PRIMARY KEY (`userId`))
ENGINE = InnoDB;

-- -----------------------------------------------------
-- Table `bgdb`.`command`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `bgdb`.`command` ;

CREATE TABLE IF NOT EXISTS `bgdb`.`command` (
  `commandId` BIGINT NOT NULL AUTO_INCREMENT,
  `commandName` VARCHAR(45) NOT NULL,
  `commandDescription` VARCHAR(200) NULL,
  `permissionLevel` INT NOT NULL DEFAULT 0,
  PRIMARY KEY (`commandId`))
ENGINE = InnoDB;

-- -----------------------------------------------------
-- Table `bgdb`.`log`
-- -----------------------------------------------------

DROP TABLE IF EXISTS `bgdb`.`log` ;

create table if not exists `bgdb`.`log` (
`LogId` bigint not null auto_increment,
`LogLevel` bigint null,
`LogDatetime` datetime null,
`CallingModule` varchar(150) null,
`FunctionName` varchar(150) null,
`LogOutput` LONGTEXT,
`Node` varchar(50) null,
`Thread` varchar(2000) null,
primary key (`logId`));

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;




-- insert data into the newly created tables
insert into bgdb.equation values (1, 'e = mc ** 2');
insert into bgdb.equation values (2, 'c ** 2 = (a ** 2) + (b ** 2)');

insert into bgdb.neuron values (1, 0, 0, 0, 'node1', 1);
insert into bgdb.neuron values (2, 0, 0, 0, 'node2', 2);

insert into bgdb.synapse values (1, 2, 2, 2, 1, 1);
insert into bgdb.synapse values (2, 3, 3, 3, 2, 2);

--
insert into bgdb.user values (1, 'bleu', 'password', 'salt', 'Brad', 'Leu', 'some notes', 'T', null, null, 5);

insert into bgdb.command values (1, 'runSim', 'run the simulation', 1);
insert into bgdb.command values (2, 'loadNeurons', 'upload neuron data', 5);

insert into bgdb.log (LogLevel, LogDateTime, CallingModule, FunctionName, LogOutput, Node, Thread) values (2, current_timestamp,
'Module1', 'GetImaginaryStuff', 'Error in GetImaginaryStuff', 'Node2', 'Thread text here');
--
--    --sample for encrypting/dectrypting passwords
--    insert into bgdb.User (user_name, user_password, first_name, last_name) values ('bleu', aes_encrypt('123456', 'bleu'), 'Brad', 'Leu');
--
--    select user_name, aes_decrypt(`User_Password`, 'secret') as password, first_name, last_name as password from bgdb.User;
