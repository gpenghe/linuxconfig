#!/bin/bash

get_repos() {
  subgroup_id=$1
  curl --silent --header "PRIVATE-TOKEN: $token" "https://gitlab.com/api/v4/groups/$subgroup_id/projects" | jq -r '.[] | [.name, .web_url] | @csv'
  subgroups=$(curl --silent --header "PRIVATE-TOKEN: $token" "https://gitlab.com/api/v4/groups/$subgroup_id/subgroups" | jq -r '.[] | .id')
  for subgroup in $subgroups; do
    get_repos $subgroup
  done
}

get_repos 54754229 
