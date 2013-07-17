#!/bin/sh

#  PreBuild.sh
#  Versioning
#
#  Created by Kozlek on 13/07/13.
#
#  Modified by RehabMan 13/07/13.
#

PROJECT_DIR=.
real_version_file="${PROJECT_DIR}/Shared/version.h"
version_file="/tmp/org_fakesmc_prebuild_version.h"
project_name=$(/usr/libexec/PlistBuddy -c "Print 'Project Name'" "${PROJECT_DIR}/version.plist")
uppercased_name=$(echo $project_name | tr [[:lower:]] [[:upper:]])
project_version=$(/usr/libexec/PlistBuddy -c "Print 'Project Version'" "${PROJECT_DIR}/version.plist")
revision_file="${PROJECT_DIR}/revision.txt"
last_revision=$(<$revision_file)

echo Last project revision: ${last_revision}

# Clean removes version.h to eventually regenerate
if [ "$1" == "clean" ]
then
    rm -f ${real_version_file}
fi

cd ${PROJECT_DIR}

sc_revision=$(svnversion)

# Fallback to git commits count
if [ "$sc_revision" == "exported" ]
then
    sc_revision=$(git rev-list --count HEAD)
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
