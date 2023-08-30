#! /bin/bash
docker stop crazysystem_client || true
docker stop crazysystem_server || true
docker stop crazysystem_drone || true
docker stop crazysystem_server_test || true
docker stop crazysystem_client_test || test
x11=$(docker stop $(docker ps -a -q  --filter ancestor=crazysystem_simulation) || true)
docker rm crazysystem_client || true
docker rm crazysystem_server || true
docker rm crazysystem_drone || true
docker rm crazysystem_server_test || true
docker rm crazysystem_client_test || true
docker rm flash || true

docker rm $x11 || true

if [ "$1" == "all" ]; then
{
    docker-compose build client server;
    docker container run -d --network host --name crazysystem_client crazysystem_client
    docker container run -v "$(pwd)"/Server/src/logs:/LOGS -v /home/:/home/ --network host --device=$2 --name crazysystem_server crazysystem_server drone
}
elif [ "$1" == "sim" ]; then
{
    if [ "$2" = "test" ]; then
    {
        docker-compose build simulation;
        x11docker $(docker images -q crazysystem_simulation) --hostdisplay --hostnet --user=RETAIN --clipboard &
        sleep 5s
        docker exec -it $(docker ps -a -q --filter ancestor=crazysystem_simulation) /bin/bash -c "cd /root/sim/; argos3 -c experiments/crazyflieReturnToBase.argos"
        sleep 2s
        docker exec -it $(docker ps -a -q --filter ancestor=crazysystem_simulation) /bin/bash -c "cd /root/sim/; argos3 -c experiments/crazyflieTakeOff.argos"
        sleep 2s
        docker exec -it $(docker ps -a -q --filter ancestor=crazysystem_simulation) /bin/bash -c "cd /root/sim/; argos3 -c experiments/crazyflieLand.argos"
        sleep 2s
        docker exec -it $(docker ps -a -q --filter ancestor=crazysystem_simulation) /bin/bash -c "cd /root/sim/; argos3 -c experiments/crazyflieExplore.argos"
    }
    else 
    {
        docker-compose build server client simulation;
        docker container run -d --network host --name crazysystem_client crazysystem_client
        docker container run -d -v "$(pwd)"/Server/src/logs:/LOGS --network host --name crazysystem_server crazysystem_server sim
        x11docker $(docker images -q crazysystem_simulation) --hostdisplay --hostnet --user=RETAIN --clipboard &
        sleep 9s
        docker ps
        docker exec -d -it $(docker ps -a -q --filter ancestor=crazysystem_simulation) /bin/bash -c "cd /root/sim/; argos3 -c experiments/crazyflieSim.argos"
    }
    fi
    
}
elif [ "$1" == "client" ]; then
{
    if [ "$2" == "test" ]; then
    {
        docker-compose build clientTest;
        docker container run --network host --name crazysystem_client_test crazysystem_client_test
    }
    elif [ "$2" == "server" ]; then
    {
        docker-compose build server;
        docker container run -d --network host --name crazysystem_client crazysystem_client
        if [ "$3" == "sim" ]; then
        {
            docker container run -d -v "$(pwd)"/Server/src/logs:/LOGS --network host --name crazysystem_server crazysystem_server sim
        }
        elif [ "$3" == "drone" ]; then
        {
            docker container run -v "$(pwd)"/Server/src/logs:/LOGS -v /home/:/home/ --network host --device=$4 --name crazysystem_server crazysystem_server drone
        }
        else
        {
            echo Wrong command, please read the README.
        }
        fi
    }
	fi
}
elif [ "$1" == "server" ]; then
{
    if [ "$2" == "sim" ]; then
    {
        docker-compose build server;
        docker container run -d -v "$(pwd)"/Server/src/logs:/LOGS -v /home/:/home/ --network host --name crazysystem_server crazysystem_server sim
    } 
	elif [ "$2" == "drone" ]; then
    {
        docker-compose build server;
        docker container run -v "$(pwd)"/Server/src/logs:/LOGS -v /home/:/home/ --network host --device=$3 --name crazysystem_server crazysystem_server drone
    }
    elif [ "$2" == "test" ]; then
    {
        docker-compose build serverTest;
        docker container run -v "$(pwd)"/Server/src/logs:/LOGS -v /home/:/home/ --network host --name crazysystem_server_test crazysystem_server_test
    }
    else
    {
        echo Wrong command, please read the README.
    }
    fi
}
elif [ "$1" == "flash" ]; then
{
    if [ "$2" == "all" ]; then
    {
        docker-compose build droneSTM32;
        docker container run --device=$3 --name flash crazysystem_drone_stm32
        docker cp flash:/root/embedded/src/cf2.bin "$(pwd)"/Drone/embedded
        echo 
        echo ===================================================================
        read -p "Please put the drone back in bootloader mode, then press enter"
        docker-compose build droneNRF;
        docker container run --device=$3 crazysystem_drone_nrf
    }
    elif [ "$2" == "simple" ]; then
    {
        docker-compose build droneSTM32;
        docker container run --device=$3 --name flash crazysystem_drone_stm32
        docker cp flash:/root/embedded/src/cf2.bin "$(pwd)"/Drone/embedded
    }
    else
    {
        echo Wrong command, please read the README.
    }
    fi
}
elif [ "$1" == "test" ]; then
{
    docker-compose build serverTest clientTest;
    docker container run -v "$(pwd)"/Server/src/logs:/LOGS -v /home/:/home/ --network host --name crazysystem_server_test crazysystem_server_test
     echo 
    echo ===================================================================
    read -p "Server tests done. Please press enter to execute client tests."
    docker container run --network host --name crazysystem_client_test crazysystem_client_test

}
else
{
    echo Wrong command, please read the README.
}
fi
