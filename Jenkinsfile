#!/usr/bin/env groovy

@Library('kanolib')
import build_deb_pkg


def repo_name = 'overlay-snapshots'


stage ('Build') {
    autobuild_repo_pkg "$repo_name"
}
