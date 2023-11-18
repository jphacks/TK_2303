CREATE TABLE `aliveCount` (
	`id` text PRIMARY KEY NOT NULL,
	`count` integer NOT NULL,
	`lastUpdate` integer
);
--> statement-breakpoint
CREATE UNIQUE INDEX `aliveCount_id_unique` ON `aliveCount` (`id`);--> statement-breakpoint
CREATE UNIQUE INDEX `aliveCountIdx` ON `aliveCount` (`id`);