java -jar ~/bin/jenkins-cli.jar -auth jenkins:jenkins -s http://192.168.4.10:8080 groovysh 'jenkins.instance.getItemByFullName("Baseboard").updateNextBuildNumber(26000)'
