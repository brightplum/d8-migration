## Purpose
This folder and manifest files provide an easy way to 
break up the migration execution of the CD site.

## Steps for Migration

1. Get a clean D8 site
1. Run the cd_migration install profile
1. Put D7 db settings config in settings.php and name it 'upgrade'
1. `drush migrate-upgrade --legacy-db-key=upgrade --configure-only`
1. `drush migrate-manifest --legacy-db-key=upgrade migration-manifests/[NAME-OF-FILE].yml`