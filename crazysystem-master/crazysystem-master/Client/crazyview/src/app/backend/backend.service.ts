import { Injectable } from '@angular/core';
import { BackendAPI } from './backend-api';
import { BackendHelperService } from './backend-helper.service';
import { ServPoint } from './interfaces/server';

@Injectable({
    providedIn: 'root',
})

/*
This class send the RESTful API requests to the backend. All of these commands
are linked to buttons.
*/

export class BackendService {

    constructor(private backendHelper: BackendHelperService) { }
 
    public async connectDrones(numberOfDrones: number): Promise<string> {
        const url = BackendAPI.connectDrones(1, numberOfDrones);
        return this.backendHelper.httpPost(url);
    }

    public async takeOffDrone(): Promise<void> {
        const url = BackendAPI.takeoff();
        await this.backendHelper.httpPost(url);
    }

    public async landDrone(): Promise<void> {
        const url = BackendAPI.land();
        await this.backendHelper.httpPost(url);
    }

    public async softwareUpdate(droneID:number, filePath:string): Promise<string> {
        const url = BackendAPI.softwareUpdate(droneID,filePath);
        return this.backendHelper.httpPost(url);
    }

    public async returnToBase(): Promise<void> {
        const url = BackendAPI.returnToBase();
        await this.backendHelper.httpPost(url);
    }

    public async getMapPointsAtStart(): Promise<ServPoint[]> {
        const url = BackendAPI.getPointsAtStart();
        return await this.backendHelper.httpGet<ServPoint[]>(url);
    }
}
