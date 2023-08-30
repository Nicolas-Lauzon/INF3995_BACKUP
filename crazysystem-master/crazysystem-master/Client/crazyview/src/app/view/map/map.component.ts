import { AfterViewInit, Component, ElementRef, OnInit, ViewChild } from '@angular/core';
import { ServDronePosition, ServPoint } from 'src/app/backend/interfaces/server';
import { SocketHelperService } from 'src/app/backend/socket-helper.service';
import { BackendService } from 'src/app/backend/backend.service';

const DRONE_POINT_RADIUS = 2;

@Component({
    selector: 'app-map',
    templateUrl: './map.component.html',
    styleUrls: ['./map.component.scss']
})
export class MapComponent implements OnInit, AfterViewInit {

    @ViewChild('droneMap', { static: true }) droneMap: ElementRef<HTMLCanvasElement>;

    private context: CanvasRenderingContext2D;
    public lastUpdate: number;
    public dronePositions: Map<number, ServPoint>;

    constructor(
        private socketHelper: SocketHelperService,
        private backend: BackendService
    ) {
        this.dronePositions = new Map<number, ServPoint>();
    }

    public async ngOnInit() {
        const initPoints = await this.backend.getMapPointsAtStart();
        this.onNewPoints(initPoints);
        this.socketHelper.socket.on(
            'new-map-points',
            (points: ServPoint[]) => this.onNewPoints(points),
        );
        this.socketHelper.socket.on(
            'drone-position',
            (dronePosition: ServDronePosition) => this.moveDrone(dronePosition.id, dronePosition.position),
        );
    }

    public async ngAfterViewInit() {
        this.context = this.droneMap.nativeElement.getContext('2d');
    }

    private onNewPoints(points: ServPoint[]): void {
        points.forEach(point => this.drawPoint(point));
        this.lastUpdate = Date.now();
    }

    private drawPoint(point: ServPoint): void {
        this.context.beginPath();
        this.context.arc(point.coordX, point.coordY, 1, 0, 2 * Math.PI, true);
        this.context.fillStyle = "black";
        this.context.fill();
    }

    private drawDrone(position: ServPoint) {
        this.context.beginPath();
        this.context.arc(position.coordX, position.coordY, DRONE_POINT_RADIUS, 0, 2 * Math.PI, true);
        this.context.fillStyle = "red";
        this.context.fill();
    }

    private async moveDrone(id: number, newPosition: ServPoint) {
        const oldPosition = this.dronePositions.get(id);
        if(oldPosition) {
            this.context.clearRect(
                oldPosition.coordX - (DRONE_POINT_RADIUS + 1),
                oldPosition.coordY - (DRONE_POINT_RADIUS + 1),
                (DRONE_POINT_RADIUS + 1) * 2,
                (DRONE_POINT_RADIUS + 1) * 2,    
            );
        }
        this.drawDrone(newPosition);
        this.dronePositions.set(id, newPosition);
    }

}
