import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatButtonModule } from '@angular/material/button';
import { MainPageComponent } from './view/main-page/main-page.component';
import { MatCardModule } from '@angular/material/card';
import { MatToolbarModule } from '@angular/material/toolbar';
import { FlexLayoutModule } from '@angular/flex-layout';
import { MatTableModule } from '@angular/material/table';
import { HttpClientModule } from '@angular/common/http';
import { MatIconModule } from '@angular/material/icon';
import { DroneInfoComponent } from './view/drone-info/drone-info.component';
import { MapComponent } from './view/map/map.component';
import { MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { FormsModule } from '@angular/forms';
import { MatInputModule } from '@angular/material/input';
import { ConnectDronesDialogComponent } from './view/main-page/connect-drones-dialog/connect-drones-dialog.component';
import { SoftwareUpdateDialogComponent } from './view/main-page/software-update-dialog/software-update-dialog.component';
import { HelpDialogComponent } from './view/main-page/help-dialog/help-dialog.component';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

@NgModule({
    declarations: [
        AppComponent,
        MainPageComponent,
        DroneInfoComponent,
        MapComponent,
        ConnectDronesDialogComponent,
        SoftwareUpdateDialogComponent,
        HelpDialogComponent,
    ],
    imports: [
        BrowserModule,
        AppRoutingModule,
        BrowserAnimationsModule,
        MatButtonModule,
        MatCardModule,
        MatToolbarModule,
        FlexLayoutModule,
        MatTableModule,
        HttpClientModule,
        MatIconModule,
        MatDialogModule,
        MatFormFieldModule,
        FormsModule,
        MatInputModule,
        MatTooltipModule,
        MatProgressSpinnerModule
    ],
    providers: [],
    bootstrap: [AppComponent],
})
export class AppModule { }
