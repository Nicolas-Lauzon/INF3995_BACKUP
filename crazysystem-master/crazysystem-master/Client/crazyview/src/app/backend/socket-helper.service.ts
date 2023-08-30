import { Injectable } from '@angular/core';
import { io, Socket } from 'socket.io-client';
import { environment } from 'src/environments/environment';
import { DroneInfo, DroneState } from './interfaces/client';
import { ServDroneInfo } from './interfaces/server';

@Injectable({
    providedIn: 'root',
})

/*
This class connects to the backend socket to receive all the info that the UI
needs about the drones and the points, etc.
*/

export class SocketHelperService {

    public socket: Socket;

    constructor() {
        this.socket = io('http://' + environment.LOCAL_IP + ':5005');
    }

    public convertDronesInfos(servDrone: ServDroneInfo): DroneInfo {
        const drone = {
            Id: servDrone.id,
            BatteryLevel: servDrone.batteryLevel,
            State: servDrone.state,
            Speed: servDrone.speed,
        };
        return drone;
    }
}
