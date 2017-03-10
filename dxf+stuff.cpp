
void WriteDXF::Write3dPOLYLINEHeader(const char* layer, int numVertices, bool isClosed /* = 0 */ )
{       
    int entity_stamp = this->GetTimestamp();
    TRACE("entity_stamp = %d\n", entity_stamp);
    char line[100];
    fprintf(fFp,"POLYLINE\n  5\n%d\n100\nAcDbEntity\n  8\n%s\n100\n", entity_stamp, layer);
    fprintf(fFp,"AcDb3dPolyline\n 66\n     1\n 10\n0.0\n 20\n0.0\n 30\n0.0\n 70\n     8\n  0\n");
}


void WriteDXF::Write3dPOLYLINE(SoCoordinate3 * coords, char * layer , int mark_interval)
{
    mark_interval = 10;
    int numCoords = coords->point.getNum();
    if(numCoords < 2) return;
    float width = 0.0;
    for(int i=0; i<numCoords; i++) {
        SbVec3f point = coords->point[i];

        if (mark_interval > 0) {
            if(i % mark_interval == 0) {
                width = 0.2;
            }
        }
        this->Write3dPOLYLINEPoint(point, width);
    }
    fprintf(fFp, "  0\n");
}

void WriteDXF::Write3dPOLYLINEPoint(SbVec3f point, float segment_width /* = 0 */, double bulge /* = 0.0 */, bool isAnArc /* = FALSE */)
{
    fprintf(fFp,"VERTEX\n  5\n");
    fprintf(fFp," 10\n");
    fprintf(fFp,"%f\n", point[0]);
    fprintf(fFp," 20\n");
    fprintf(fFp,"%f\n", point[1]);

    // spec width for each segment even if zero...
    fprintf(fFp, " 40\n");
    fprintf(fFp, "%f\n", segment_width);
    fprintf(fFp, " 41\n");
    fprintf(fFp, "%f\n", segment_width);

    if (isAnArc == TRUE) {
        fprintf(fFp, " 42\n");
        fprintf(fFp, "%f\n", bulge);
    }
}
