DEV_ALIAS = commondreams.develop
PROD_ALIAS = commondreams.master
LOCAL_ALIAS = commondreams._local

sync-db-dev:
	drush sql-sync @$(DEV_ALIAS) @$(LOCAL_ALIAS)

sync-db-prod:
	drush sql-sync @$(PROD_ALIAS) @$(LOCAL_ALIAS)

sync-files-dev:
	drush rsync @$(DEV_ALIAS):sites/default/files docroot/sites/default/ --mode=razv --delete

sync-files-pvt-dev:
	drush rsync @$(DEV_ALIAS):/app/files-private . --mode=razv --delete

sync-files-prod:
	drush rsync @$(PROD_ALIAS):sites/default/files docroot/sites/default/ --mode=razv --delete

sync-files-pvt-prod:
	drush rsync @$(PROD_ALIAS):/app/files-private . --mode=razv --delete

composer-rebuild:
	drush composer-json-rebuild

