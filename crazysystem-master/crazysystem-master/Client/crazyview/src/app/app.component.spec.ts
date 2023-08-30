import { TestBed } from '@angular/core/testing';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MatDialogModule } from '@angular/material/dialog';
import { MatIconModule } from '@angular/material/icon';
import { MatToolbarModule } from '@angular/material/toolbar';
import { RouterTestingModule } from '@angular/router/testing';
import { AppComponent } from './app.component';
import { BackendHelperService } from './backend/backend-helper.service';
import { DroneInfoComponent } from './view/drone-info/drone-info.component';
import { MainPageComponent } from './view/main-page/main-page.component';
import { MapComponent } from './view/map/map.component';

describe('AppComponent', () => {
    let backendHelperServiceSpy: jasmine.SpyObj<BackendHelperService>;

    beforeEach(async () => {
        backendHelperServiceSpy = jasmine.createSpyObj(
            'BackendHelperService',
            [
                'httpGet',
                'httpPost',
            ],
        );

        await TestBed.configureTestingModule({
        imports: [
            RouterTestingModule,
            MatToolbarModule,
            MatButtonModule,
            MatCardModule,
            MatDialogModule,
            MatIconModule,
        ],
        declarations: [
            AppComponent,
            MainPageComponent,
            DroneInfoComponent,
            MapComponent,
        ],
        providers: [
            { provide: BackendHelperService, useValue: backendHelperServiceSpy },
        ],
        }).compileComponents();
    });

    it('should create the app', () => {
        const fixture = TestBed.createComponent(AppComponent);
        const app = fixture.componentInstance;
        expect(app).toBeTruthy();
    });

    it(`should have as title 'crazyview'`, () => {
        const fixture = TestBed.createComponent(AppComponent);
        const app = fixture.componentInstance;
        expect(app.title).toEqual('crazyview');
    });
});
