DEV_ALIAS = dqi.develop
PROD_ALIAS = dqi.master
LOCAL_ALIAS = dqi._local

sync-db-dev:
	drush sql-sync @$(DEV_ALIAS) @$(LOCAL_ALIAS)

sync-db-prod:
	drush sql-sync @$(PROD_ALIAS) @$(LOCAL_ALIAS)

sync-files-dev:
	drush rsync @dqi.develop:sites/default/files docroot/sites/default/ --mode=razv --delete

sync-files-pvt-dev:
	drush rsync @dqi.develop:/app/files-private . --mode=razv --delete

sync-files-prod:
	drush rsync @dqi.master:sites/default/files docroot/sites/default/ --mode=razv --delete

sync-files-pvt-prod:
	drush rsync @dqi.master:/app/files-private . --mode=razv --delete

composer-rebuild:
	drush composer-json-rebuild

daylog:
	git log --pretty=format:"- %s" --since="`date +"%Y-%m-%d"` 0:0:0" --author=`git config user.email`

