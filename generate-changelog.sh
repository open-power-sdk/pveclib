#!/bin/bash
#
# Generate markdown ChangeLog files based on git history.

tmp=$(mktemp)
touch ${tmp}

prev_tag=""
echo "# ChangeLog"
for tag in $(git tag -l v[0-9]* | sort -V); do
	# Prepend tags to the output because we want the ChangeLog in the
	# reverse order, e.g. 2.0.0, 1.0.1 and 1.0.0.
	{
		date=$(git show --date=short --format='tformat:%cd' -s ${tag})
		echo -e "\n## ${tag} - ${date}\n"
		git --no-pager log --no-merges --format='format: - %s%n' \
		    ${prev_tag:+${prev_tag}..}${tag} | cat - ${tmp}
	} > ${tmp}.1
	mv ${tmp}.1 ${tmp}
	prev_tag=${tag}
done

# Send the output to stdout.
cat ${tmp}

rm ${tmp}
