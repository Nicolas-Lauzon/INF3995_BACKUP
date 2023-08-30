# Crazyview

---
**NOTE**

We recommend to use our ./run.sh at all times! The instructions here 'should' work, but we don't guarantee it.

Nous recommandons d'utilisez notre script ./run.sh tout le temps! Les instructions ici 'devraient' fonctionner, mais nous ne pouvons rien garantir!

---

Projet genere avec : [Angular CLI](https://github.com/angular/angular-cli) version 11.1.2.<br>

## Development serveur

Executer `ng serve` pour lancer un serveur de developement. Naviguer a `http://localhost:4200/` pour observer l'application web.<br>

## Rouler les tests unitaires

Pour les tests, utilisez le `./run.sh`! / To run the tests, please use the `./run.sh` command!
<br>
<br>

Executer `ng test` pour rouler les tests unitaires.<br>

To build test container, run: / Pour construire le container des tests:<br>
`docker build -f Dockerfile.test -t crazysystem_client_test . --network host --build-arg FILES_TO_COPY=/crazyview`<br>

Then, to run the tests: / Puis, pour rouler les tests:<br>
`docker container run --network host --name crazysystem_client_test crazysystem_client_test`<br>

## Rouler les tests "end-to-end"

Executer `ng e2e` pour rouler les tests end-to-end via [Protractor](http://www.protractortest.org/).<br>


## Running dockerfile / Rouler le dockerfile

To build container, run: / Pour construire le container:<br>
`docker build -f Dockerfile.client -t crazysystem_client . --network host --build-arg FILES_TO_COPY=/crazyview`<br>

Then, to run the app: / Puis, pour rouler l'appli:<br>
`docker container run --network host --name crazysystem_client crazysystem_client`<br>