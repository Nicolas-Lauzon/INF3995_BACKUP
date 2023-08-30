import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { environment } from 'src/environments/environment';

@Injectable({
    providedIn: 'root',
})

/*
This class connects to the RESTful API backend to send (POST) the requests.
*/

export class BackendHelperService {

    private readonly mainUrl = 'http://' + environment.LOCAL_IP + ':5000/';

    constructor(private httpClient: HttpClient) { }

    public httpPost<T = object>(url: string, data: T = null): Promise<any> {
        const headers = { 'content-type': 'application/json' };
        return this.httpClient.post(this.mainUrl + url, data, { headers }).toPromise();
    }

    public httpGet<T = object>(url: string): Promise<T> {
        return this.httpClient.get<T>(this.mainUrl + url).toPromise<T>();
    }
}
