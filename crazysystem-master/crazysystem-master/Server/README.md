# Crazysystem/Server

---
**NOTE**

We recommend to use our ./run.sh at all times! The instructions here 'should' work, but we don't guarantee it.

Nous recommandons d'utilisez notre script ./run.sh tout le temps! Les instructions ici 'devraient' fonctionner, mais nous ne pouvons rien garantir!

---

## Run dockerfile / Rouler le dockerfile

first, build the container with: / D'abord, construisez le container:<br>
`docker build -f Dockerfile.server -t crazysystem_server . --network host --build-arg FILES_TO_COPY=/src`<br>

Then, you can run the container, either in "drone" mode or in "sim" mode: / Puis, roulez le container, soit en mode "drone", soit en mode "sim":<br>
`docker container run -v "$(pwd)"/src/logs:/LOGS -v /home/:/home/ --network host --device=path/to/device --name crazysystem_server crazysystem_server drone`<br>
OR<br>
`docker container run -v "$(pwd)"/logs:/LOGS -v /home/:/home/ --network host --name crazysystem_server crazysystem_server sim`<br>

NOTE: The path to device looks like this: /dev/bus/usb/...<br>
      Le chemin de la radio ressemble à cela: /dev/bus/usb/...<br>

## Run tests

Pour les tests, utilisez le `./run.sh`! / To run the tests, please use the `./run.sh` command!
<br>
<br>

first, build the container with: / D'abord, construisez le container:<br>
`docker build -f Dockerfile.test -t crazysystem_server_test . --network host --build-arg FILES_TO_COPY=/src`<br>

Then, to run the tests: / Puis, pour rouler les tests:<br>
`docker container run -v "$(pwd)"/logs:/LOGS -v /home/:/home/ --network host --name crazysystem_server_test crazysystem_server_test`
