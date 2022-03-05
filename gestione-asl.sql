-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema gestione_asl
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `gestione_asl` ;

-- -----------------------------------------------------
-- Schema gestione_asl
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `gestione_asl` DEFAULT CHARACTER SET utf8 ;
USE `gestione_asl` ;

-- -----------------------------------------------------
-- Table `gestione_asl`.`paziente`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`paziente` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`paziente` (
  `num_ts` INT NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `cognome` VARCHAR(45) NOT NULL,
  `data_nascita` DATE NOT NULL,
  `luogo_nascita` VARCHAR(45) NOT NULL,
  `residenza` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`num_ts`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `gestione_asl`.`recapito`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`recapito` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`recapito` (
  `paziente` INT NOT NULL,
  `campo` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`paziente`, `campo`),
  CONSTRAINT `Paziente`
    FOREIGN KEY (`paziente`)
    REFERENCES `gestione_asl`.`paziente` (`num_ts`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB
COMMENT = '		';


-- -----------------------------------------------------
-- Table `gestione_asl`.`tipo_esame`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`tipo_esame` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`tipo_esame` (
  `nome` VARCHAR(45) NOT NULL,
  `disponibile` TINYINT NOT NULL,
  PRIMARY KEY (`nome`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `gestione_asl`.`esame_eseguito`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`esame_eseguito` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`esame_eseguito` (
  `codice` INT NOT NULL,
  `data` DATE NOT NULL,
  `prenotazione` INT NOT NULL,
  `paziente` INT NOT NULL,
  `tipo` VARCHAR(45) NOT NULL,
  `medico` VARCHAR(16) NOT NULL,
  `diagnosi` VARCHAR(256) NULL,
  PRIMARY KEY (`codice`),
  CONSTRAINT `fk_esame_eseguito_paz`
    FOREIGN KEY (`paziente`)
    REFERENCES `gestione_asl`.`paziente` (`num_ts`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_esame_eseguito_tipo`
    FOREIGN KEY (`tipo`)
    REFERENCES `gestione_asl`.`tipo_esame` (`nome`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_esame_eseguito_paz_idx` ON `gestione_asl`.`esame_eseguito` (`paziente` ASC);

CREATE INDEX `fk_esame_eseguito_tipo_idx` ON `gestione_asl`.`esame_eseguito` (`tipo` ASC);


-- -----------------------------------------------------
-- Table `gestione_asl`.`reparto`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`reparto` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`reparto` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `codice` INT NOT NULL,
  `ospedale` INT NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `telefono` VARCHAR(45) NOT NULL,
  `primario` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_reparto_osp`
    FOREIGN KEY (`ospedale`)
    REFERENCES `gestione_asl`.`ospedale` (`codice`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_reparto_primario`
    FOREIGN KEY (`primario`)
    REFERENCES `gestione_asl`.`personale` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;

CREATE INDEX `fk_reparto_osp_idx` ON `gestione_asl`.`reparto` (`ospedale` ASC);

CREATE INDEX `fk_reparto_primario_idx` ON `gestione_asl`.`reparto` (`primario` ASC);

CREATE UNIQUE INDEX `uq_rep_osp` ON `gestione_asl`.`reparto` (`codice` ASC, `ospedale` ASC);


-- -----------------------------------------------------
-- Table `gestione_asl`.`utenti`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`utenti` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`utenti` (
  `username` VARCHAR(45) NOT NULL,
  `password` CHAR(32) NOT NULL,
  `ruolo` ENUM('amministratore', 'cup', 'personale') NOT NULL,
  PRIMARY KEY (`username`))
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `gestione_asl`.`personale`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`personale` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`personale` (
  `cf` VARCHAR(16) NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `cognome` VARCHAR(45) NOT NULL,
  `domicilio` VARCHAR(45) NOT NULL,
  `tipo` ENUM('medico', 'primario', 'volontario') NOT NULL,
  `reparto` INT NOT NULL,
  `username` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`cf`),
  CONSTRAINT `fk_personale_reparto`
    FOREIGN KEY (`reparto`)
    REFERENCES `gestione_asl`.`reparto` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_personale_username`
    FOREIGN KEY (`username`)
    REFERENCES `gestione_asl`.`utenti` (`username`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_personale_reparto_idx` ON `gestione_asl`.`personale` (`reparto` ASC);

CREATE INDEX `fk_personale_username_idx` ON `gestione_asl`.`personale` (`username` ASC);


-- -----------------------------------------------------
-- Table `gestione_asl`.`ospedale`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`ospedale` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`ospedale` (
  `codice` INT NOT NULL AUTO_INCREMENT,
  `nome` VARCHAR(45) NOT NULL,
  `indirizzo` VARCHAR(45) NOT NULL,
  `responsabile` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`codice`),
  CONSTRAINT `fk_ospedale_resp`
    FOREIGN KEY (`responsabile`)
    REFERENCES `gestione_asl`.`personale` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;

CREATE INDEX `fk_ospedale_resp_idx` ON `gestione_asl`.`ospedale` (`responsabile` ASC);


-- -----------------------------------------------------
-- Table `gestione_asl`.`laboratorio`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`laboratorio` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`laboratorio` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `codice` INT NOT NULL,
  `ospedale` INT NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `piano` INT NOT NULL,
  `stanza` INT NOT NULL,
  `responsabile` VARCHAR(16) NOT NULL,
  PRIMARY KEY (`id`),
  CONSTRAINT `fk_laboratorio_osp`
    FOREIGN KEY (`ospedale`)
    REFERENCES `gestione_asl`.`ospedale` (`codice`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_laboratorio_resp`
    FOREIGN KEY (`responsabile`)
    REFERENCES `gestione_asl`.`personale` (`cf`)
    ON DELETE NO ACTION
    ON UPDATE CASCADE)
ENGINE = InnoDB;

CREATE INDEX `fk_laboratorio_osp_idx` ON `gestione_asl`.`laboratorio` (`ospedale` ASC);

CREATE INDEX `fk_laboratorio_resp_idx` ON `gestione_asl`.`laboratorio` (`responsabile` ASC);

CREATE UNIQUE INDEX `uq_lab_osp` ON `gestione_asl`.`laboratorio` (`codice` ASC, `ospedale` ASC);


-- -----------------------------------------------------
-- Table `gestione_asl`.`esame_prenotato`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`esame_prenotato` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`esame_prenotato` (
  `codice` INT NOT NULL AUTO_INCREMENT,
  `data` DATE NOT NULL,
  `ora` TIME NOT NULL,
  `costo` FLOAT NOT NULL,
  `urgente` TINYINT NOT NULL,
  `prenotazione` INT NULL,
  `paziente` INT NOT NULL,
  `tipo` VARCHAR(45) NOT NULL,
  `laboratorio` INT NOT NULL,
  PRIMARY KEY (`codice`),
  CONSTRAINT `fk_esame_prenotato_paz`
    FOREIGN KEY (`paziente`)
    REFERENCES `gestione_asl`.`paziente` (`num_ts`)
    ON DELETE CASCADE
    ON UPDATE CASCADE,
  CONSTRAINT `fk_esame_prenotato_tipo`
    FOREIGN KEY (`tipo`)
    REFERENCES `gestione_asl`.`tipo_esame` (`nome`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_esame_prenotato_lab`
    FOREIGN KEY (`laboratorio`)
    REFERENCES `gestione_asl`.`laboratorio` (`id`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;

CREATE INDEX `fk_esame_prenotato_paz_idx` ON `gestione_asl`.`esame_prenotato` (`paziente` ASC);

CREATE INDEX `fk_esame_prenotato_tipo_idx` ON `gestione_asl`.`esame_prenotato` (`tipo` ASC);

CREATE INDEX `fk_esame_prenotato_lab_idx` ON `gestione_asl`.`esame_prenotato` (`laboratorio` ASC);

CREATE UNIQUE INDEX `uq_paziente_tipo_data` ON `gestione_asl`.`esame_prenotato` (`paziente` ASC, `tipo` ASC, `data` ASC)  COMMENT 'RA1 un paziente non puo prenotare lo stesso tipo di esame nella stessa data';


-- -----------------------------------------------------
-- Table `gestione_asl`.`parametro`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`parametro` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`parametro` (
  `esame` INT NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  `valore` DOUBLE NOT NULL,
  `misura` VARCHAR(45) NULL,
  PRIMARY KEY (`esame`, `nome`),
  CONSTRAINT `fk_param_esame`
    FOREIGN KEY (`esame`)
    REFERENCES `gestione_asl`.`esame_eseguito` (`codice`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `gestione_asl`.`specializzazione`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`specializzazione` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`specializzazione` (
  `primario` VARCHAR(16) NOT NULL,
  `disciplina` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`primario`, `disciplina`),
  CONSTRAINT `fk_specializzazione_personale`
    FOREIGN KEY (`primario`)
    REFERENCES `gestione_asl`.`personale` (`cf`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `gestione_asl`.`associazione`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestione_asl`.`associazione` ;

CREATE TABLE IF NOT EXISTS `gestione_asl`.`associazione` (
  `volontario` VARCHAR(16) NOT NULL,
  `nome` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`volontario`),
  CONSTRAINT `fk_associazione_vol`
    FOREIGN KEY (`volontario`)
    REFERENCES `gestione_asl`.`personale` (`cf`)
    ON DELETE CASCADE
    ON UPDATE CASCADE)
ENGINE = InnoDB;

USE `gestione_asl` ;

-- -----------------------------------------------------
-- procedure inserisci_recapito
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_recapito`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_recapito` (IN num_ts INT, IN recapito VARCHAR(45))
BEGIN
    insert into `recapito` values(num_ts, recapito);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure login
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`login`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `login` (in var_username varchar(45), in var_pass varchar(45), out var_role int)
BEGIN
	declare var_user_role enum('amministratore', 'cup', 'personale');
    
    select `ruolo` from `utenti`
    where `username` = var_username
    and `password` = md5(var_pass)
    into var_user_role;
    
    -- See the corrisponding enum in the client
		if var_user_role = 'amministratore' then
			set var_role = 1;
		elseif var_user_role = 'cup' then
			set var_role = 2;
		elseif var_user_role = 'personale' then
			set var_role = 3;
		else
			set var_role = 4;
		end if;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure crea_utente
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`crea_utente`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `crea_utente` (IN username VARCHAR(45), IN pass VARCHAR(45), IN ruolo varchar(45))
BEGIN
	insert into utenti VALUES(username, MD5(pass), ruolo);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_paziente
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`cancella_paziente`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `cancella_paziente` (in var_num_ts int)
BEGIN
	declare patient_exists int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level repeatable read;
    start transaction;
		select count(num_ts) into patient_exists from `paziente` where `num_ts` = var_num_ts;
        
        if patient_exists = 1 then
			delete from `paziente` where `num_ts` = var_num_ts;
		else
			signal sqlstate '45003' set message_text = 'Deleting a non existent patient';
        end if;
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure prenota_esame
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`prenota_esame`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `prenota_esame` (in var_data date, in var_ora time, in var_costo float, in var_urgente boolean, inout var_prenotazione int, in var_paziente int, in var_tipo varchar(45), in var_id_lab int)
BEGIN
	declare is_available int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
	set transaction isolation level serializable;
    start transaction;
		select `disponibile` into is_available
        from `tipo_esame`
        where `nome` = var_tipo;
        
        if is_available = 0 then
			signal sqlstate '45000' set message_text = 'This type of exam is not avialable';
		end if;
        
		insert into `esame_prenotato` values (null, var_data, var_ora, var_costo, var_urgente, null, var_paziente, var_tipo, var_id_lab);
    
    -- this is a work-aroud to have esame_prenotato.prenotazione like a second auto-increment attribute using the AI PK esame_prenotato.codice
		if var_prenotazione is null then -- the only exam in a reservation OR the first of multiple in the same reservation
			update `esame_prenotato`
            set `prenotazione` = LAST_INSERT_ID()
            where `codice` = LAST_INSERT_ID();
            set var_prenotazione = LAST_INSERT_ID(); -- return the reservation number given
		else -- if va_prenotazione is not null => reserving an exam "inside" a reservation with multiple exams with reservation number = var_prenotazione
			update `esame_prenotato`
            set `prenotazione` = var_prenotazione
            where `codice` = LAST_INSERT_ID();
		end if;

    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_tipo_esame
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_tipo_esame`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_tipo_esame` (in nome varchar(45))
BEGIN
	insert into `tipo_esame` values (nome, true);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure stato_tipo_esame
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`stato_tipo_esame`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `stato_tipo_esame` (in var_tipo_esame varchar(45), in disponibile boolean)
BEGIN
	declare exist int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
	set transaction isolation level read committed;
	start transaction;
		
        select count(*) into exist
        from `tipo_esame`
        where `nome` = var_tipo_esame;
        
        if exist <> 1 then
			signal sqlstate '45000' set message_text = 'This type of exam does not exists';
		end if;
        
		update `tipo_esame`
		set `disponibile` = disponibile
		where `nome` = var_tipo_esame;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure esami_disponibili
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`esami_disponibili`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `esami_disponibili` ()
BEGIN
	set transaction isolation level read committed;
    start transaction;
        select `nome` as 'Tipo Esame' from `tipo_esame` where `disponibile` = true;
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure lista_laboratori
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`lista_laboratori`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `lista_laboratori` ()
BEGIN
	set transaction isolation level read committed;
    start transaction;
        select `laboratorio`.`codice` as 'Cod. Lab.', `laboratorio`.`nome` as 'Laboratorio', `laboratorio`.`ospedale` as 'Cod. Osp.', `ospedale`.`nome` as 'Ospedale', `laboratorio`.`id` as 'ID Laboratorio'
        from `laboratorio` join `ospedale` on `laboratorio`.`ospedale` = `ospedale`.`codice`;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_recapito
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`cancella_recapito`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `cancella_recapito` (in var_num_ts int, in var_campo varchar(45))
BEGIN
	declare num_recapiti int;
    declare contact_exists int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level repeatable read;
    start transaction;
		-- check if the contact the user want to delete actually exists
		select count(campo) into contact_exists	from `recapito` where `paziente` = var_num_ts and `campo` = var_campo;
		if contact_exists < 1 then
			signal sqlstate '45001' set message_text = 'The contact you are trying to delete does not exists';
		end if;
		
		select count(paziente) from `recapito` where `paziente` = var_num_ts into num_recapiti;
		if num_recapiti > 1 then
			delete from `recapito` where `paziente` = var_num_ts and `campo` = var_campo;
		else
			signal sqlstate '45002' set message_text = 'Deleting the only contact for this patient is forbidden';
		end if;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_paziente
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_paziente`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_paziente` (in num_ts int, in nome varchar(45), in cognome varchar(45), in data_nascita date, in luogo_nascita varchar(45), in residenza varchar(45), in recapito varchar(45))
BEGIN
    declare exit handler for sqlexception
    begin
		rollback; -- rollback any changes made in the transaction
        resignal; -- raise again the sql exception to the caller
	end;
    
    start transaction;
		insert into `paziente` values (num_ts, nome, cognome, data_nascita, luogo_nascita, residenza);
        insert into `recapito` values (num_ts, recapito);
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure anagrafica_paziente
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`anagrafica_paziente`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `anagrafica_paziente` (in var_num_ts int)
BEGIN
	set transaction isolation level read committed;
	start transaction;
		select `num_ts` as 'Numero TS', `nome` as 'Nome', `cognome` as 'Cognome', `data_nascita` as 'Data nasc.', `luogo_nascita` as 'Luogo di nascita', `residenza` as 'Residenza'
		from `paziente` where `num_ts` = var_num_ts;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure lista_recapiti
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`lista_recapiti`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `lista_recapiti` (in var_paziente int)
BEGIN
	set transaction isolation level read committed;
    start transaction;
		select `campo` as 'Recapiti' from `recapito` where `paziente` = var_paziente;
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure modifica_paziente
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`modifica_paziente`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `modifica_paziente` (in var_num_ts int, in colonna int, in var_new_num_ts int, in var_str varchar(45), in var_date date)
BEGIN
    declare exist int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level repeatable read;
    start transaction;
		
        select count(*) into exist
        from `paziente`
        where `num_ts` = var_num_ts;
        
        if exist < 1 then
			signal sqlstate '45004' set message_text = 'The patient you are trying to update does not exists';
		end if;
        
		if colonna = 1 then
			update `paziente`
			set `num_ts` = var_new_num_ts
			where `num_ts` = var_num_ts;
		elseif colonna = 2 then
			update `paziente`
			set `nome` = var_str
			where `num_ts` = var_num_ts;
		elseif colonna = 3 then
			update `paziente`
			set `cognome` = var_str
			where `num_ts` = var_num_ts;
		elseif colonna = 4 then
			update `paziente`
			set `data_nascita` = var_date
			where `num_ts` = var_num_ts;
		elseif colonna = 5 then
			update `paziente`
			set `luogo_nascita` = var_str
			where `num_ts` = var_num_ts;
		elseif colonna = 6 then
			update `paziente`
			set `residenza` = var_str
			where `num_ts` = var_num_ts;
		else
			signal sqlstate '45005' set message_text = 'The second parameter must be between 1 and 6 both included';
		end if;
	
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_prenotazione
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`report_prenotazione`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `report_prenotazione` (in var_prenotazione int)
BEGIN
    declare var_esame int;
    
    declare done int default false;
    declare cur cursor for select `codice` from `esame_eseguito` where `prenotazione` = var_prenotazione order by `codice` desc;
    declare continue handler for not found set done = true;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
        
        -- 1) esami prenotati
		-- the exams reserved and not executed yet with reservation number = var_prenotazione
		select `tipo`, `paziente`.`nome`, `paziente`.`cognome`, `data`, `ora`, `laboratorio`.`nome`, `ospedale`.`nome`, `urgente`, `costo`
		from `esame_prenotato` join `paziente` on `esame_prenotato`.`paziente` = `paziente`.`num_ts`
        join `laboratorio` on `esame_prenotato`.`laboratorio` = `laboratorio`.`id`
        join `ospedale` on `laboratorio`.`ospedale` = `ospedale`.`codice`
		where `prenotazione` = var_prenotazione;
        
        -- 2) loop esami eseguiti to print their results
			-- a) get a table   esame|data|medico|diagnosi   for this patient
        select `esame_eseguito`.`tipo`, `data`, `personale`.`nome`, `personale`.`cognome`, `diagnosi`, `paziente`.`nome`, `paziente`.`cognome`
        from `esame_eseguito` join `paziente` on `esame_eseguito`.`paziente` = `paziente`.`num_ts` 
        join `personale` on `esame_eseguito`.`medico` = `personale`.`cf`
        where `prenotazione` = var_prenotazione
        order by `codice` desc;
        
			-- b) for each esame eseguito print the relative table   parametri|valori
        open cur;
        read_loop: loop
			
            fetch cur into var_esame;
            if done then
				leave read_loop;
			end if;
            
            select `nome` , `valore`, `misura`
            from `parametro`
            where `esame` = var_esame;
		
		end loop;
        close cur;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_storico_paz
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`report_storico_paz`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `report_storico_paz` (in var_num_ts int)
BEGIN
    declare var_esame int;
    
    declare done int default false;
    declare cur cursor for select `codice` from `esame_eseguito` where `paziente` = var_num_ts order by `codice` desc;
    declare continue handler for not found set done = true;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
        
        -- 1) get a table   esame|data|medico|diagnosi   for this patient
        select `esame_eseguito`.`tipo`, `data`, `personale`.`nome`, `personale`.`cognome`, `diagnosi`
        from `esame_eseguito` join `personale` on `esame_eseguito`.`medico` = `personale`.`cf`
        where `paziente` = var_num_ts
        order by `codice` desc;
        
        -- 2) for each esame eseguito print the relative table   parametri|valori
        open cur;
        read_loop: loop
			
            fetch cur into var_esame;
            if done then
				leave read_loop;
			end if;
            
            select `nome` , `valore`, `misura`
            from `parametro`
            where `esame` = var_esame;
		
		end loop;
        close cur;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiorna_diagnosi
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`aggiorna_diagnosi`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `aggiorna_diagnosi` (in var_codice_esame int, in var_username varchar(45), in var_diagnosi varchar(256))
BEGIN
    
    declare var_caller varchar(16);
    declare var_medico varchar(16);
    declare exist int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level repeatable read;
    start transaction;
		
        select count(*) into exist
        from `esame_eseguito`
        where `codice` = var_codice_esame;
        
        if exist <> 1 then
			signal sqlstate '45000' set message_text = 'An exam with this code does not exist or is not yet executed';
		end if;
        
		-- 1. check if that personnel member actually executed that exam
		select `cf` into var_caller
		from `personale`
		where `username` = var_username;
		
		select `medico` into var_medico
		from `esame_eseguito`
		where `codice` = var_codice_esame;
		
		if var_caller <> var_medico then
			signal sqlstate '45009' set message_text = 'You cannot modify this exam because you did not executed it';
		end if;
		
		-- 2. update
		update `esame_eseguito`
		set `diagnosi` = var_diagnosi
		where `codice` = var_codice_esame;

	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure esegui_esame
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`esegui_esame`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `esegui_esame` (in var_codice int, in var_username varchar(45), in var_diagnosi varchar(256))
BEGIN

	declare var_ruolo enum('amministratore', 'cup', 'personale');
	declare exist int;
    declare var_medico varchar(16);
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level repeatable read;
    start transaction;
    
        -- check if exists the reservation of this exam
		select count(*) into exist
        from `esame_prenotato`
        where `codice` = var_codice;
        
        if exist <> 1 then
			signal sqlstate '45000' set message_text = 'This exam does not exist or is already executed';
		end if;
        
        -- check if the caller has 'personale' role
		select `ruolo` into var_ruolo
		from `utenti`
		where `username` = var_username;
		
		if var_ruolo <> 'personale' then
			signal sqlstate '45008' set message_text = 'You are not a personnel member';
		end if;
        
        
        -- get the doc's fiscal code
        select `cf` into var_medico
        from `personale`
        where `username` = var_username;
        
        -- delete from reservations and insert in executed exams
		insert into `esame_eseguito` (codice, `data`, prenotazione, paziente, tipo, medico, diagnosi)
			select `esame_prenotato`.`codice`, `esame_prenotato`.`data`, `esame_prenotato`.`prenotazione`, `esame_prenotato`.`paziente`, `esame_prenotato`.`tipo`, var_medico, var_diagnosi
			from `esame_prenotato`
			where `esame_prenotato`.`codice` = var_codice;
		
		delete from `esame_prenotato` where `codice` = var_codice;
    
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_risultati
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_risultati`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_risultati` (in var_esame int, in var_parametro varchar(45), in var_valore double, in var_misura varchar(45))
BEGIN
	insert into `parametro` values(var_esame, var_parametro, var_valore, var_misura);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_ospedale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_ospedale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_ospedale` (in var_nome_osp varchar(45), in var_ind_osp varchar(45) , in var_medico varchar(16), in var_nome_rep varchar(45), in var_tel_rep varchar(45), in var_specializzazione varchar(45), out osp_code int)
BEGIN
	
    declare var_tipo enum('medico', 'primario', 'volontario');
    declare is_resp_lab int;
    declare is_resp_osp int;
    
	declare exit handler for sqlexception
    begin
		rollback; -- rollback any changes made in the transaction
        resignal; -- raise again the sql exception to the caller
	end;
    
    set transaction isolation level serializable;
    start transaction;
	-- seleziona personale di tipo medico che non sia responsabile di qualcosa
	-- crea un ospedale e assegna quel dottore come responsabile
	-- crea un nuovo reparto e assegna quel dottore come primario
	-- aggiorna il tipo e il reparto del dottore ora primario
	-- assegna una specializzazione
		
        -- check if is type 'medico'
        select `tipo` into var_tipo
        from `personale`
        where `cf` = var_medico;
        
        if var_tipo <> 'medico' then
			signal sqlstate '45000' set message_text = 'The personnel chosen is not type medico';
        end if;
        
        -- check the doctor is not responsable of a hospital or a lab
        select count(*) into is_resp_lab
        from `laboratorio`
        where `responsabile` = var_medico;
        
        select count(*) into is_resp_osp
        from `ospedale`
        where `responsabile` = var_medico;
        
        if is_resp_lab + is_resp_osp > 0 then
			signal sqlstate '45000' set message_text = 'Cannot move this doctor because she/he is already responsible somewhere else';
		end if;
		
		-- add the hospital
        insert into `ospedale` values (null, var_nome_osp, var_ind_osp, var_medico);
        
        -- add the ward
        set osp_code = last_insert_id();
        insert into `reparto` values(null, 1, last_insert_id(), var_nome_rep, var_tel_rep, var_medico);
		
        -- update the doctor
        update `personale`
		set `reparto` = last_insert_id(), `tipo` = 'primario'
		where `cf` = var_medico;
        
        -- add a degree
        insert into `specializzazione` values (var_medico, var_specializzazione);
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_personale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_personale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_personale` (in var_cf varchar(16), in var_nome varchar(45), in var_cognome varchar(45), in var_domicilio varchar(45), in var_tipo varchar(45), in var_reparto int, in var_username varchar(45))
BEGIN
    insert into `personale` values (var_cf, var_nome, var_cognome, var_domicilio, var_tipo, var_reparto, var_username);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure anagrafica_personale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`anagrafica_personale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `anagrafica_personale` (in var_cf varchar(16))
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
	set transaction isolation level repeatable read;
	start transaction;
		select `cf` as 'Cod. Fiscale', `nome` as 'Nome', `cognome` as 'Cognome', `domicilio` as 'Domicilio', `tipo` as 'Tipo', `reparto` as 'Reparto', `username`
		from `personale` where `cf` = var_cf;
        
        select `tipo` into var_tipo
        from `personale` where `cf` = var_cf;
        
        -- print the volounteer's "associazione" or the degrees of the ward responsible
        if var_tipo = 'primario' then
			select `disciplina` as 'Specializzazione'
            from `specializzazione` where `primario` = var_cf;
        elseif var_tipo = 'volontario' then
			select `nome` as 'Associazione di Volontariato'
            from `associazione` where `volontario` = var_cf;
        end if;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure modifica_personale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`modifica_personale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `modifica_personale` (in var_cf varchar(16), in colonna int, in var_str varchar(45), in var_reparto int)
BEGIN
	declare exist int;
    declare is_resp_lab int;
    declare is_resp_osp int;
    declare is_primario int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;  -- is count(*) protected?
    start transaction;
		
        select count(*) into exist
        from `personale`
        where `cf` = var_cf;
        
        if exist < 1 then
			signal sqlstate '45000' set message_text = 'The personnel member you are trying to update does not exists';
		end if;
        
	-- check that not(colonna=5 or 6 AND not responsable of a lab/ward/hospital)
		select count(*) into is_resp_lab
        from `laboratorio`
        where `responsabile` = var_cf;
        
        select count(*) into is_resp_osp
        from `ospedale`
        where `responsabile` = var_cf;
        
        select count(*) into is_primario
        from `reparto`
        where `primario` = var_cf;
        
        if (is_resp_lab + is_resp_osp + is_primario > 0) AND (colonna = 5 or colonna = 6) then
			signal sqlstate '45001' set message_text = 'Cannot update this attr because she/he is responsible for something. Move this personnel member first.';
		end if;

        
		if colonna = 1 then
			update `personale`
			set `cf` = var_str
			where `cf` = var_cf;
		elseif colonna = 2 then
			update `personale`
            set `nome` = var_str
            where `cf` = var_cf;
		elseif colonna = 3 then
			update `personale`
			set `cognome` = var_str
			where `cf` = var_cf;
		elseif colonna = 4 then
			update `personale`
			set `domicilio` = var_str
			where `cf` = var_cf;
		elseif colonna = 5 then
			update `personale`
			set `tipo` = var_str
			where `cf` = var_cf;
		elseif colonna = 6 then
			update `personale`
			set `reparto` = var_reparto
			where `cf` = var_cf;
		elseif colonna = 7 then
			update `personale`
			set `username` = var_str
			where `cf` = var_cf;
		else
			signal sqlstate '45002' set message_text = 'The second parameter must be between 1 and 7 both included';
		end if;
	
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_personale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`cancella_personale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `cancella_personale` (in var_cf varchar(16))
BEGIN
	declare exist int;
    declare is_resp_lab int;
    declare is_resp_osp int;
    declare is_primario int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
		
        select count(*) into exist
        from `personale`
        where `cf` = var_cf;
        
        if exist < 1 then
			signal sqlstate '45000' set message_text = 'The personnel member you are trying to update does not exists';
		end if;
        
        -- check if is responsible of a hospital/laboratory/ward
		select count(*) into is_resp_lab
        from `laboratorio`
        where `responsabile` = var_cf;
        
        select count(*) into is_resp_osp
        from `ospedale`
        where `responsabile` = var_cf;
        
        select count(*) into is_primario
        from `reparto`
        where `primario` = var_cf;
        
        if (is_resp_lab + is_resp_osp + is_primario > 0) then
			signal sqlstate '45001' set message_text = 'Cannot delete this personnel member because she/he is responsible for something. Move this personnel member first.';
		end if;
        
        delete from `personale` where `cf` = var_cf;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure report_esami_eseguiti
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`report_esami_eseguiti`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `report_esami_eseguiti` (in var_cf varchar(16), in opzione int)
BEGIN
	-- opzione = 0 => monthly report; opzione = 1 => annual report
    declare var_time_length date;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level read committed;
    start transaction;
    
		if opzione = 0 then
			select curdate() - interval 30 day into var_time_length;
		elseif opzione = 1 then
			select curdate() - interval 365 day into var_time_length;
		else
			signal sqlstate '45000' set message_text = 'Second parameter must be 0 or 1';
		end if;
		
		select `codice`, `tipo`, `data`, `paziente`.`num_ts` as 'TS Paziente', `diagnosi`
        from `esame_eseguito` join `paziente` on `esame_eseguito`.`paziente` = `paziente`.`num_ts`
        where `medico` = var_cf and `data` > var_time_length;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure info_ospedale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`info_ospedale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `info_ospedale` (in var_codice int)
BEGIN
	set transaction isolation level read committed;
    start transaction;
		select * from `ospedale` where `codice` = var_codice;
		select * from `reparto` where `ospedale` = var_codice;
        select * from `laboratorio` where `ospedale` = var_codice;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure lista_ospedali
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`lista_ospedali`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `lista_ospedali` ()
BEGIN
	set transaction isolation level read committed;
    start transaction;
        select `ospedale`.`codice` as 'Cod. Ospedale', `ospedale`.`nome` as 'Nome Ospedale'
        from `ospedale`;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure modifica_ospedale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`modifica_ospedale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `modifica_ospedale` (in var_codice int, in colonna int, in var_str varchar(45))
BEGIN
	declare exist int;
    declare hosp_of_new_resp int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;  -- how is count(*) protected?
    start transaction;
		
        select count(*) into exist
        from `ospedale`
        where `codice` = var_codice;
        
        if exist < 1 then
			signal sqlstate '45000' set message_text = 'The hospital you are trying to update does not exists';
		end if;
        
        -- check that if colonna = 3 (i.e. update responsable), the doc "pointed" works in a ward inside the hospital
        if colonna = 3 then
			select `ospedale`.`codice` into hosp_of_new_resp
			from `personale` join `reparto` on `personale`.`reparto` = `reparto`.`id`
			join `ospedale` on `reparto`.`ospedale` = `ospedale`.`codice`
			where `personale`.`cf` = var_str;
			
			if var_codice <> hosp_of_new_resp then
				signal sqlstate '45001' set message_text = 'Cannot update this attr because this doctor does not work in this hospital.';
			end if;
		end if;

        
		if colonna = 1 then
			update `ospedale`
			set `nome` = var_str
			where `codice` = var_codice;
		elseif colonna = 2 then
			update `ospedale`
            set `indirizzo` = var_str
            where `codice` = var_codice;
		elseif colonna = 3 then
			update `ospedale`
			set `responsabile` = var_str
			where `codice` = var_codice;
		else
			signal sqlstate '45002' set message_text = 'The second parameter must be between 1 and 3 both included';
		end if;
	
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_ospedale
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`cancella_ospedale`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `cancella_ospedale` (in var_codice int, out var_new_reparto int)
BEGIN
	declare exist int;
    declare num_osp int;
    declare new_reparto int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
	set transaction isolation level serializable;
    -- repeatable read is not enough because reads from a snapshot
    -- so num_osp can be not consistent with the state of the DB
    -- and is important to have it consistent to prevent the deletion of the only hospital in the ASL
    start transaction;
		-- check if the hospital exists and is not the only hospital in the ASL
        select count(*) into exist
        from `ospedale`
        where `codice` = var_codice;
        
        if exist < 1 then
			signal sqlstate '45000' set message_text = 'This hospital does not exist';
        end if;
        
        select count(*) into num_osp
        from `ospedale`;
        
        if num_osp < 2 then
 			signal sqlstate '45000' set message_text = 'Cannot delete the last hospital';
		end if;
        
        -- find a hospital and a ward to move the personnel of the hospital being deleted
        select max(`id`) into new_reparto
        from `reparto`
        where `reparto`.`ospedale` = (select max(`codice`) from `ospedale` where `codice` <> var_codice);
        
        set var_new_reparto = new_reparto;
        
        -- convert ward responsibles ('primario' type) in 'medico', delete their "degrees" first
        delete from `specializzazione`
 		where `primario` in (	select `cf`
 									from `personale` join `reparto` on `personale`.`reparto` = `reparto`.`id` -- join `ospedale` on `reparto`.`ospedale` = `ospedale`.`codice`
                                    where `reparto`.`ospedale` = var_codice -- `ospedale`.`codice` = var_codice
 								);
		
        update `personale`
        set `tipo` = 'medico'
        where `tipo` = 'primario' and `reparto` in (	select `id`
 														from `reparto`
 														where `reparto`.`ospedale` = var_codice
 													);
        
		-- move the personnel
 		update `personale`
 		set `reparto` = new_reparto
 		where `reparto` in (	select `id`
 								from `reparto`
                                where `reparto`.`ospedale` = var_codice
 							);
        
        -- delete the hospital, the laboratories and wards will be deleted on cascade
        delete from `ospedale` where `codice` = var_codice;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiorna_associazione_vol
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`aggiorna_associazione_vol`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `aggiorna_associazione_vol` (in var_cf varchar(16), in var_associazione varchar(45))
BEGIN
    declare exist int;
    
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level repeatable read;
    start transaction;
		select count(*) into exist
        from `associazione`
        where `volontario` = var_cf;
        
        if exist = 0 then
			insert into `associazione` values (var_cf, var_associazione);
        elseif exist = 1 then
			update `associazione`
			set `nome` = var_associazione
			where `volontario` = var_cf; 
        end if;
        
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure aggiungi_specializzazione
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`aggiungi_specializzazione`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `aggiungi_specializzazione` (in var_cf varchar(16), in var_specializzazione varchar(45))
BEGIN
	insert into `specializzazione` values (var_cf, var_specializzazione);
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_reparto
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_reparto`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_reparto` (in var_osp int, in var_nome varchar(45), in var_tel varchar(45), in var_resp varchar(16), in var_specializzazione varchar(45))
BEGIN
	declare var_osp_check int;
    declare var_tipo enum('medico', 'primario', 'volontario');
    declare var_codice_rep int;
	
    declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
        select `reparto`.`ospedale`, `personale`.`tipo` into var_osp_check, var_tipo
        from `personale` join `reparto` on `personale`.`reparto` = `reparto`.`id`
        where `personale`.`cf` = var_resp;
        
        if var_osp <> var_osp_check then
			signal sqlstate '45000' set message_text = 'The doctor must work in the same hospital of the new ward';
        end if;
        
        if var_tipo <> 'medico' then
			signal sqlstate '45000' set message_text = 'reparto.primario cannot be a personnel with "volontario" type or someone already responsible of a ward';
        end if;
        
        -- update var_resp to 'primario' type, add a degree
        update `personale`
		set `tipo` = 'primario'
		where `cf` = var_resp;
        
        insert into `specializzazione` values (var_resp, var_specializzazione);
        
        -- find a value for 'codice' and add the ward
        select max(codice) + 1 into var_codice_rep
        from `reparto`
        where `ospedale` = var_osp;
        
        insert into `reparto` values (null, var_codice_rep, var_osp, var_nome, var_tel, var_resp);
        
        -- move the 'primario' to the new ward
        update `personale`
		set `reparto` = last_insert_id()
		where `cf` = var_resp;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure modifica_reparto
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`modifica_reparto`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `modifica_reparto` (in var_id int, in colonna int, in var_str varchar(45), in var_specializzazione varchar(45))
BEGIN
	declare exist int;
    declare var_tipo enum('medico', 'primario', 'volontario');
    declare var_osp int;
    declare osp_new_resp int;
    declare old_primario varchar(16);
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
		
        select count(*) into exist
        from `reparto`
        where `id` = var_id;
        
        if exist <> 1 then
			signal sqlstate '45000' set message_text = 'this ward does not exist';
        end if;
        
        if colonna = 1 then
			update `reparto` set `nome` = var_str where `id` = var_id; 
        elseif colonna = 2 then
			update `reparto` set `telefono` = var_str where `id` = var_id; 
        elseif colonna = 3 then
			
            select `ospedale`, `tipo` into osp_new_resp, var_tipo
            from `personale` join `reparto` on `personale`.`reparto` = `reparto`.`id`
            where `personale`.`cf` = var_str;
            
            select `ospedale` into var_osp
            from `reparto`
            where `id` = var_id;
            
            if var_tipo <> 'medico' then
				signal sqlstate '45000' set message_text = 'the new ward must be "medico" type';
            end if;
            
            if var_osp <> osp_new_resp then
				signal sqlstate '45000' set message_text = 'the new ward responsible works in a different hospital';
            end if;

            -- update new ward responsable type and assigned ward, and add a degree
            update `personale` set `tipo` = 'primario', `reparto` = var_id where `cf` = var_str;
            insert into `specializzazione` values (var_str, var_specializzazione);
            
            -- remember the old ward responsable
            select `primario` into old_primario
            from `reparto`
            where `id` = var_id;
            
            -- update the responsable attribute in 'reparto'
            update `reparto` set `primario` = var_str where `id` = var_id;
            
            -- update the old ward responsable type and delete her/his degrees
            delete from `specializzazione` where `primario` = old_primario;
            update `personale` set `tipo` = 'medico' where `cf` = old_primario;
            
        end if;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_reparto
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`cancella_reparto`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `cancella_reparto` (in var_id int, out var_new_reparto int)
BEGIN
	declare exist int;
    declare var_osp int;
    declare num_reparti int;
    declare new_reparto int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
	set transaction isolation level serializable;
    -- repeatable read is not enough because reads from a snapshot
    -- so num_reparti can be not consistent with the state of the DB
    -- and is important to have it consistent to prevent the deletion of the only ward in a hospital
    start transaction;
        -- check if the ward exist and is not the only one in its hospital
        select count(*) into exist
        from `reparto`
        where `id` = var_id;
        
        if exist < 1 then
			signal sqlstate '45000' set message_text = 'This ward does not exist';
        end if;
        
        select `ospedale` into var_osp
		from `reparto`
		where `id` = var_id;
        
        select count(*) into num_reparti
        from `reparto`
        where `ospedale` = var_osp;
        
        if num_reparti < 2 then
 			signal sqlstate '45000' set message_text = 'Cannot delete the last ward in the hospital';
		end if;
        
        -- find a ward to move the personnel in the ward being deleted
        select max(`id`) into new_reparto
        from `reparto`
        where `reparto`.`ospedale` = var_osp and `id` <> var_id;
        
        set var_new_reparto = new_reparto;
        
        -- convert the responisble in 'medico', delete her/his degrees first
        delete from `specializzazione`
 		where `primario` in (	select `cf`
 								from `personale`
                                where `reparto` = var_id
							);
		
        update `personale`
        set `tipo` = 'medico'
        where `tipo` = 'primario' and `reparto` = var_id;
        
        -- move the personnel in the other ward
 		update `personale`
 		set `reparto` = new_reparto
 		where `reparto` = var_id;
        
        delete from `reparto` where `id` = var_id;
        
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure inserisci_laboratorio
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`inserisci_laboratorio`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `inserisci_laboratorio` (in var_osp int, in var_nome varchar(45), in var_piano int, in var_stanza int, in var_resp varchar(16))
BEGIN
	declare var_tipo enum('medico', 'volontario', 'primario');
    declare ospedale_resp int;
    declare is_resp int;
    declare new_cod int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
		
        -- check if var_resp is not 'volontario'
        select `tipo` into var_tipo
        from `personale`
        where `cf` = var_resp;
        
        if var_tipo = 'volontario' then
			signal sqlstate '45000' set message_text = 'the new responsible cannot be type "volontario"';
        end if;
        
        -- check if var_resp works in the same hospital of the lab
        select `ospedale` into ospedale_resp
        from `personale` join `reparto` on `personale`.`reparto` = `reparto`.`id`
        where `cf` = var_resp;
        
        if var_osp <> ospedale_resp then
			signal sqlstate '45000' set message_text = 'the new responsible cannot work in another hospital';
        end if;
        
        -- check var_resp is not already responsable of a lab
        select count(*) into is_resp
        from `laboratorio`
        where `responsabile` = var_resp;
        
        if is_resp <> 0 then
			signal sqlstate '45000' set message_text = 'the new responsible is already responsible of a lab';
        end if;
        
        -- find the 'codice' to assign to the new lab
        select max(codice) + 1 into new_cod
        from `laboratorio`
        where `ospedale` = var_osp;
        -- if can't find a 'codice', set 1
        if new_cod is null then
			set new_cod = 1;
		end if;
        
        insert into `laboratorio` values (null, new_cod, var_osp, var_nome, var_piano, var_stanza, var_resp);
    commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure modifica_laboratorio
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`modifica_laboratorio`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `modifica_laboratorio` (in var_id int, in colonna int, in var_str varchar(45), in var_int int)
BEGIN
	declare var_tipo enum('medico', 'volontario', 'primario');
    declare ospedale_resp int;
    declare ospedale_lab int;
    declare is_resp int;
    declare lab_exist int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    start transaction;
		-- check if var_id exist
        select count(*) into lab_exist
        from `laboratorio`
        where `id` = var_id;
        
        if lab_exist < 1 then
			signal sqlstate '45000' set message_text = 'this laboratory does not exist';
        end if;
        
        if colonna = 1 then
			update `laboratorio` set `nome` = var_str where `id` = var_id;
        elseif colonna = 2 then
			update `laboratorio` set `piano` = var_int where `id` = var_id;
        elseif colonna = 3 then
			update `laboratorio` set `stanza` = var_int where `id` = var_id;
        elseif colonna = 4 then
			-- check new responsable is not 'volontario' type
			select `tipo` into var_tipo
			from `personale`
			where `cf` = var_str;
			
			if var_tipo = 'volontario' then
				signal sqlstate '45000' set message_text = 'the new responsible cannot be type "volontario"';
			end if;
			
			-- check if she/he works in the same hospital of the lab
			select `ospedale` into ospedale_resp
			from `personale` join `reparto` on `personale`.`reparto` = `reparto`.`id`
			where `cf` = var_str;
			
            select `ospedale` into ospedale_lab
            from `laboratorio`
            where `id` = var_id;
            
			if ospedale_lab <> ospedale_resp then
				signal sqlstate '45000' set message_text = 'the new responsible cannot work in another hospital';
			end if;
			
			-- check she/he is not already responsable of a lab
			select count(*) into is_resp
			from `laboratorio`
			where `responsabile` = var_str;
			
			if is_resp <> 0 then
				signal sqlstate '45000' set message_text = 'the new responsible is already responsible of a lab';
			end if;
            
            update `laboratorio`
            set `responsabile` = var_str
            where `id` = var_id;
		end if;
	commit;
END$$

DELIMITER ;

-- -----------------------------------------------------
-- procedure cancella_laboratorio
-- -----------------------------------------------------

USE `gestione_asl`;
DROP procedure IF EXISTS `gestione_asl`.`cancella_laboratorio`;

DELIMITER $$
USE `gestione_asl`$$
CREATE PROCEDURE `cancella_laboratorio` (in var_id int)
BEGIN
	declare num_lab int;
    declare var_osp int;
    
	declare exit handler for sqlexception
    begin
		rollback;
        resignal;
	end;
    
    set transaction isolation level serializable;
    -- repeatable read is not enough because reads from a snapshot
    -- so num_lab can be not consistent with the state of the DB
    -- and is important to have it consistent to prevent the deletion of the only lab in a hospital
    start transaction;
		
        select `ospedale` into var_osp
        from `laboratorio`
        where `id` = var_id;
        
        select count(*) into num_lab
        from `laboratorio`
        where `ospedale` = var_osp;
        
        if num_lab < 2 then
			signal sqlstate '45000' set message_text = 'the last lab in the hospital cannot be deleted';
        end if;
        
        delete from `laboratorio` where `id` = var_id;
        
    commit;
END$$

DELIMITER ;
USE `gestione_asl`;

DELIMITER $$

USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`personale_BEFORE_INSERT` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`personale_BEFORE_INSERT` BEFORE INSERT ON `personale` FOR EACH ROW
BEGIN
	declare var_ruolo enum('amministratore', 'cup', 'personale');
    
    select `ruolo` into var_ruolo
    from `utenti`
    where `utenti`.`username` = new.`username`;
    
    if var_ruolo <> 'personale' then
		signal sqlstate '45000'
        set message_text = 'This user has not role "personale" so cannot be linked with a personnel member';
	end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`personale_BEFORE_UPDATE` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`personale_BEFORE_UPDATE` BEFORE UPDATE ON `personale` FOR EACH ROW
BEGIN
	declare var_ruolo enum('amministratore', 'cup', 'personale');
    
    select `ruolo` into var_ruolo
    from `utenti`
    where `utenti`.`username` = new.`username`;
    
    if var_ruolo <> 'personale' then
		signal sqlstate '45000'
        set message_text = 'This user has not role "personale" so cannot be linked with a personnel member';
	end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`personale_AFTER_UPDATE` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`personale_AFTER_UPDATE` AFTER UPDATE ON `personale` FOR EACH ROW
BEGIN
	if old.`tipo` = 'primario' AND new.`tipo` <> 'primario' then
		delete from `specializzazione`
        where `primario` = old.`cf`;
    end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`esame_prenotato_BEFORE_INSERT` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`esame_prenotato_BEFORE_INSERT` BEFORE INSERT ON `esame_prenotato` FOR EACH ROW
BEGIN
	-- date must be in the future
    if new.data <= curdate() then
		signal sqlstate '45000'
        set message_text = 'Reserving an exam today or in the past is impossible';
	end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`specializzazione_BEFORE_INSERT` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`specializzazione_BEFORE_INSERT` BEFORE INSERT ON `specializzazione` FOR EACH ROW
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
    select `tipo` into var_tipo
    from `personale`
    where `cf` = new.`primario`;
    
    if var_tipo <> 'primario' then
		signal sqlstate '45000' set message_text = 'The personnel memeber is not "primario" type';
	end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`specializzazione_BEFORE_UPDATE` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`specializzazione_BEFORE_UPDATE` BEFORE UPDATE ON `specializzazione` FOR EACH ROW
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
    select `tipo` into var_tipo
    from `personale`
    where `cf` = new.`primario`;
    
    if var_tipo <> 'primario' then
		signal sqlstate '45000' set message_text = 'The personnel memeber is not "primario" type';
	end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`associazione_BEFORE_INSERT` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`associazione_BEFORE_INSERT` BEFORE INSERT ON `associazione` FOR EACH ROW
BEGIN
    declare var_tipo enum('medico', 'primario', 'volontario');
    
    select `tipo` into var_tipo
    from `personale`
    where `cf` = new.`volontario`;
    
    if var_tipo <> 'volontario' then
		signal sqlstate '45000' set message_text = 'The personnel memeber is not "volontario" type';
	end if;
END$$


USE `gestione_asl`$$
DROP TRIGGER IF EXISTS `gestione_asl`.`associazione_BEFORE_UPDATE` $$
USE `gestione_asl`$$
CREATE DEFINER = CURRENT_USER TRIGGER `gestione_asl`.`associazione_BEFORE_UPDATE` BEFORE UPDATE ON `associazione` FOR EACH ROW
BEGIN
	declare var_tipo enum('medico', 'primario', 'volontario');
    
    select `tipo` into var_tipo
    from `personale`
    where `cf` = new.`volontario`;
    
    if var_tipo <> 'volontario' then
		signal sqlstate '45000' set message_text = 'The personnel memeber is not "volontario" type';
	end if;
END$$


DELIMITER ;
SET SQL_MODE = '';
GRANT USAGE ON *.* TO cup;
 DROP USER cup;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'cup' IDENTIFIED BY 'cup';

GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_recapito` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`cancella_paziente` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`esami_disponibili` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`prenota_esame` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`lista_laboratori` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_paziente` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`anagrafica_paziente` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`lista_recapiti` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`modifica_paziente` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`cancella_recapito` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`report_prenotazione` TO 'cup';
GRANT EXECUTE ON procedure `gestione_asl`.`report_storico_paz` TO 'cup';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO login;
 DROP USER login;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'login' IDENTIFIED BY 'login';

GRANT EXECUTE ON procedure `gestione_asl`.`login` TO 'login';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO amministratore;
 DROP USER amministratore;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'amministratore' IDENTIFIED BY 'amministratore';

GRANT EXECUTE ON procedure `gestione_asl`.`crea_utente` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_tipo_esame` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`stato_tipo_esame` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_ospedale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_personale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`anagrafica_personale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`modifica_personale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`cancella_personale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`report_esami_eseguiti` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`info_ospedale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`lista_ospedali` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`modifica_ospedale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`cancella_ospedale` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`aggiorna_associazione_vol` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`aggiungi_specializzazione` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_reparto` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`modifica_reparto` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`cancella_reparto` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_laboratorio` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`modifica_laboratorio` TO 'amministratore';
GRANT EXECUTE ON procedure `gestione_asl`.`cancella_laboratorio` TO 'amministratore';
SET SQL_MODE = '';
GRANT USAGE ON *.* TO personale;
 DROP USER personale;
SET SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';
CREATE USER 'personale' IDENTIFIED BY 'personale';

GRANT EXECUTE ON procedure `gestione_asl`.`aggiorna_diagnosi` TO 'personale';
GRANT EXECUTE ON procedure `gestione_asl`.`esegui_esame` TO 'personale';
GRANT EXECUTE ON procedure `gestione_asl`.`inserisci_risultati` TO 'personale';

-- -----------------------------------------------------
-- Data for table `gestione_asl`.`paziente`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`paziente` (`num_ts`, `nome`, `cognome`, `data_nascita`, `luogo_nascita`, `residenza`) VALUES (1, 'mario', 'rossi', '2000-01-01', 'roma', 'roma');
INSERT INTO `gestione_asl`.`paziente` (`num_ts`, `nome`, `cognome`, `data_nascita`, `luogo_nascita`, `residenza`) VALUES (2, 'Giuseppe', 'Verdi', '2002-02-02', 'Le Roncole', 'Via Giuseppe Verdi 1, Milano');
INSERT INTO `gestione_asl`.`paziente` (`num_ts`, `nome`, `cognome`, `data_nascita`, `luogo_nascita`, `residenza`) VALUES (10, 'Francesco', 'Totti', '1976-09-27', 'Roma', 'Roma');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`recapito`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`recapito` (`paziente`, `campo`) VALUES (1, '555555555');
INSERT INTO `gestione_asl`.`recapito` (`paziente`, `campo`) VALUES (1, '555000555');
INSERT INTO `gestione_asl`.`recapito` (`paziente`, `campo`) VALUES (2, '222222222');
INSERT INTO `gestione_asl`.`recapito` (`paziente`, `campo`) VALUES (10, '333101010');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`tipo_esame`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`tipo_esame` (`nome`, `disponibile`) VALUES ('Ecografia', 1);
INSERT INTO `gestione_asl`.`tipo_esame` (`nome`, `disponibile`) VALUES ('Analisi del sangue', 1);
INSERT INTO `gestione_asl`.`tipo_esame` (`nome`, `disponibile`) VALUES ('TAC', 1);
INSERT INTO `gestione_asl`.`tipo_esame` (`nome`, `disponibile`) VALUES ('ECG', 1);
INSERT INTO `gestione_asl`.`tipo_esame` (`nome`, `disponibile`) VALUES ('ecocolordoppler', 0);
INSERT INTO `gestione_asl`.`tipo_esame` (`nome`, `disponibile`) VALUES ('PCR covid19', 1);

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`esame_eseguito`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`esame_eseguito` (`codice`, `data`, `prenotazione`, `paziente`, `tipo`, `medico`, `diagnosi`) VALUES (2, '2021-02-03', 2, 2, 'Analisi del sangue', 'fcanhn', NULL);
INSERT INTO `gestione_asl`.`esame_eseguito` (`codice`, `data`, `prenotazione`, `paziente`, `tipo`, `medico`, `diagnosi`) VALUES (3, '2021-02-02', 2, 2, 'PCR covid19', 'fcanhn', 'il paziente risulta positivo e deve isolarsi in quarantena fino a doppio tampone negativo');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`reparto`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`reparto` (`id`, `codice`, `ospedale`, `nome`, `telefono`, `primario`) VALUES (1, 1, 1, 'Virologia', '0612345', 'fcanhn');
INSERT INTO `gestione_asl`.`reparto` (`id`, `codice`, `ospedale`, `nome`, `telefono`, `primario`) VALUES (2, 1, 2, 'Ematologia', '061122', 'klsbbo');
INSERT INTO `gestione_asl`.`reparto` (`id`, `codice`, `ospedale`, `nome`, `telefono`, `primario`) VALUES (3, 2, 2, 'Pronto Soccorso', '06118', 'cxoprc');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`utenti`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('cup', '0c88028bf3aa6a6a143ed846f2be1ea4', 'cup');
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('admin', '0c88028bf3aa6a6a143ed846f2be1ea4', 'amministratore');
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('fauci', '0c88028bf3aa6a6a143ed846f2be1ea4', 'personale');
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('kelso', '0c88028bf3aa6a6a143ed846f2be1ea4', 'personale');
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('cox', '0c88028bf3aa6a6a143ed846f2be1ea4', 'personale');
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('volontario', '0c88028bf3aa6a6a143ed846f2be1ea4', 'personale');
INSERT INTO `gestione_asl`.`utenti` (`username`, `password`, `ruolo`) VALUES ('dottore', '0c88028bf3aa6a6a143ed846f2be1ea4', 'personale');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`personale`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`personale` (`cf`, `nome`, `cognome`, `domicilio`, `tipo`, `reparto`, `username`) VALUES ('fcanhn', 'Anthony', 'Fauci', 'USA', 'primario', 1, 'fauci');
INSERT INTO `gestione_asl`.`personale` (`cf`, `nome`, `cognome`, `domicilio`, `tipo`, `reparto`, `username`) VALUES ('klsbbo', 'Bob', 'Kelso', 'USA', 'primario', 2, 'kelso');
INSERT INTO `gestione_asl`.`personale` (`cf`, `nome`, `cognome`, `domicilio`, `tipo`, `reparto`, `username`) VALUES ('cxoprc', 'Perry', 'Cox', 'USA', 'primario', 3, 'cox');
INSERT INTO `gestione_asl`.`personale` (`cf`, `nome`, `cognome`, `domicilio`, `tipo`, `reparto`, `username`) VALUES ('vlnsnt', 'Santino', 'Volontario', 'Roma', 'volontario', 1, 'volontario');
INSERT INTO `gestione_asl`.`personale` (`cf`, `nome`, `cognome`, `domicilio`, `tipo`, `reparto`, `username`) VALUES ('dottore', 'dottore', 'dottore', 'via dei medici, 1', 'medico', 1, 'dottore');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`ospedale`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`ospedale` (`codice`, `nome`, `indirizzo`, `responsabile`) VALUES (1, 'San Giovanni', 'via dell amba aradam', 'fcanhn');
INSERT INTO `gestione_asl`.`ospedale` (`codice`, `nome`, `indirizzo`, `responsabile`) VALUES (2, 'Tor Vergata', 'viale Oxford', 'klsbbo');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`laboratorio`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`laboratorio` (`id`, `codice`, `ospedale`, `nome`, `piano`, `stanza`, `responsabile`) VALUES (1, 1, 1, 'Tamponi virologia', 3, 20, 'dottore');
INSERT INTO `gestione_asl`.`laboratorio` (`id`, `codice`, `ospedale`, `nome`, `piano`, `stanza`, `responsabile`) VALUES (2, 1, 2, 'lab. ematologia', 0, 1, 'klsbbo');
INSERT INTO `gestione_asl`.`laboratorio` (`id`, `codice`, `ospedale`, `nome`, `piano`, `stanza`, `responsabile`) VALUES (3, 2, 2, 'Radiografia', 1, 1, 'cxoprc');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`esame_prenotato`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`esame_prenotato` (`codice`, `data`, `ora`, `costo`, `urgente`, `prenotazione`, `paziente`, `tipo`, `laboratorio`) VALUES (1, '2022-01-01', '11:35', 26.50, 0, 1, 1, 'ECG', 2);
INSERT INTO `gestione_asl`.`esame_prenotato` (`codice`, `data`, `ora`, `costo`, `urgente`, `prenotazione`, `paziente`, `tipo`, `laboratorio`) VALUES (4, '2021-10-10', '09:45', 123.45, 0, 2, 2, 'TAC', 1);
INSERT INTO `gestione_asl`.`esame_prenotato` (`codice`, `data`, `ora`, `costo`, `urgente`, `prenotazione`, `paziente`, `tipo`, `laboratorio`) VALUES (5, '2021-11-11', '12:34', 10, 1, 2, 1, 'ECG', 1);

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`parametro`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`parametro` (`esame`, `nome`, `valore`, `misura`) VALUES (2, 'emoglobina', 0.123456789, '%');
INSERT INTO `gestione_asl`.`parametro` (`esame`, `nome`, `valore`, `misura`) VALUES (2, 'colesterolo', 25.8, 'mg/dL');
INSERT INTO `gestione_asl`.`parametro` (`esame`, `nome`, `valore`, `misura`) VALUES (2, 'piastrine', 175, 'migliaia/mL');
INSERT INTO `gestione_asl`.`parametro` (`esame`, `nome`, `valore`, `misura`) VALUES (3, 'rna sarscov2', 1, NULL);
INSERT INTO `gestione_asl`.`parametro` (`esame`, `nome`, `valore`, `misura`) VALUES (2, 'globuli bianchi', 123456789, NULL);

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`specializzazione`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`specializzazione` (`primario`, `disciplina`) VALUES ('fcanhn', 'Virologia');
INSERT INTO `gestione_asl`.`specializzazione` (`primario`, `disciplina`) VALUES ('klsbbo', 'ematologia');
INSERT INTO `gestione_asl`.`specializzazione` (`primario`, `disciplina`) VALUES ('cxoprc', 'cardiochirurgia');

COMMIT;


-- -----------------------------------------------------
-- Data for table `gestione_asl`.`associazione`
-- -----------------------------------------------------
START TRANSACTION;
USE `gestione_asl`;
INSERT INTO `gestione_asl`.`associazione` (`volontario`, `nome`) VALUES ('vlnsnt', 'onlus medici in pensione');

COMMIT;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
