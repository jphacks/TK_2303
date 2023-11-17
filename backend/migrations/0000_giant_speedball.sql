CREATE TABLE `devices` (
	`id` text PRIMARY KEY NOT NULL,
	`name` text,
	`registerDate` integer NOT NULL,
	`status` text DEFAULT 'unknown' NOT NULL,
	`temperature` real,
	`humidity` real,
	`pressure` real,
	`lastUpdate` integer
);
--> statement-breakpoint
CREATE UNIQUE INDEX `devices_id_unique` ON `devices` (`id`);--> statement-breakpoint
CREATE UNIQUE INDEX `idx` ON `devices` (`id`);