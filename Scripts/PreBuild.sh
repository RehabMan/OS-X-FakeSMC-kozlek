#!/bin/sh

#  PreBuild.sh
#  Versioning
#
#  Created by Kozlek on 13/07/13.
#
#  Modified by RehabMan 13/07/13.
#

real_version_file="./Shared/version.h"
revision_file="./revision.txt"
version_file="/tmp/org_fakesmc_prebuild_version.h"

if [ "$1" == "clean" ]
then
    rm -f ${version_file}
    rm -f ${revision_file}
    #exit 0
fi

project_name="HWSensors"
uppercased_name=$(echo $project_name | tr [[:lower:]] [[:upper:]])

if [ -f $revision_file ]
then
    last_revision=$(<$revision_file)
else
    last_revision="-"
fi

echo Last project revision: ${last_revision}

cd .

sc_revision=$(svn info 2>/dev/null | grep ^Revision: | tr -Cd '[:digit:]')
project_version=$(svn info 2>/dev/null | grep ^URL: | awk -F\/ '{ print $NF }')

# Fallback to git commits count
svn info >/dev/null 2>&1
if [ $? != 0 ]
then
    sc_revision=$(git rev-list --count HEAD)
    project_version=$(git describe --tags)
fi

# Generate prospective new version.h
echo "" > ${version_file}
echo "#define ${uppercased_name}_REVISION ${sc_revision}" >> ${version_file}
echo "" >> ${version_file}
echo "#define ${uppercased_name}_VERSION ${project_version}.${sc_revision}" >> ${version_file}
echo "" >> ${version_file}
echo "#define ${uppercased_name}_VERSION_STRING \"${project_version}.${sc_revision}\"" >> ${version_file}
echo "" >> ${version_file}

# Only write new version.h if it is different
if [ -e ${real_version_file} ]; then
    diff_version=`diff ${version_file} ${real_version_file}`
fi

if [ ! -e "${real_version_file}" ] || [ "${diff_version}" != "" ]; then
    echo New project version: ${uppercased_name}_VERSION ${project_version}.${sc_revision}
    cp ${version_file} ${real_version_file}
fi

if [ "$last_revision" != "$sc_revision" ]
then
    echo New project revision: ${sc_revision}

    echo "${sc_revision}" > ${revision_file}

    #echo "" > ${version_file}
    #echo "#define ${uppercased_name}_REVISION ${sc_revision}" >> ${version_file}
    #echo "" >> ${version_file}
    #echo "#define ${uppercased_name}_VERSION ${project_version}.${sc_revision}" >> ${version_file}
    #echo "#define ${uppercased_name}_VERSION_STRING \"${project_version}.${sc_revision}\"" >> ${version_file}
fi
