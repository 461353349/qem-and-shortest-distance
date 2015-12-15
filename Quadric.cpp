#include "stdafx.h"
#include "Quadric.h"

Quadrics::Quadrics() {};
Quadrics::~Quadrics() {};

//��ȡfilePath��obj�ļ����������ø�obj�ļ��е����ݳ�ʼ��
//vertices��faces�ֶΣ�obj�ļ�Ҫ��ֻ������������Ϣ
void Quadrics::readObj(std::string filePath) {
    if(filePath.empty() == true) {
        printf("readObj�����Ƿ���\n");
        return;
    }

    FILE *file;
    errno_t err = fopen_s(&file, &filePath[0], "r");
    if (err != 0) {
        fprintf(stderr, "read_smf() failed: can't open data file \"%s\".\n", &filePath[0]);
        system("PAUSE");
        exit(-1);
    }
    parse(file);
    fclose(file);
}
//����obj�ļ��е�����
void Quadrics::parse(FILE* file) {
    if(file == NULL)
        return;

    char ch;
    char buf[1024];
    Vertex v;
    Face f;
    int local_num_vertices = 0;
    double x0, y0, z0;        /* ax + by + cz = 0 */
    double x1, y1, z1;
    double x2, y2, z2;
    double a, b, c, M;

    while ( fscanf(file, "%c", &ch) != EOF ) {
        switch(ch) {
        case ' ' :         /* blanks */
        case '\t':
        case '\n':
            continue;//continue��while��Ч��switch��ֻ��break��Ч
        case '#':/* comment */
        case 'g':
            fgets(buf, sizeof(buf), file);//fgets�������з�Ϊֹ������buf����
            break;
        case 'v':          /* vertex */
        case 'V':
            local_num_vertices++;							/* vertex index starts from 1 */
            fscanf_s(file, "%lf %lf %lf",	&v.x, &v.y, &v.z);
            vertices.insert(Vertices::value_type(local_num_vertices, v));
            break;
        case 'f':          /* face */
        case 'F':
            fscanf_s(file, "%d %d %d", &f.idVertex[0], &f.idVertex[1], &f.idVertex[2]);
            x0 = vertices[f.idVertex[0]].x;
            y0 = vertices[f.idVertex[0]].y;
            z0 = vertices[f.idVertex[0]].z;
            x1 = vertices[f.idVertex[1]].x;
            y1 = vertices[f.idVertex[1]].y;
            z1 = vertices[f.idVertex[1]].z;
            x2 = vertices[f.idVertex[2]].x;
            y2 = vertices[f.idVertex[2]].y;
            z2 = vertices[f.idVertex[2]].z;
            a = (y1-y0)*(z2-z0) - (z1-z0)*(y2-y0);   /* a1*b2 - a2*b1;        */
            b = (z1-z0)*(x2-x0) - (x1-x0)*(z2-z0);   /* a2*b0 - a0*b2;        */
            c = (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0);   /* a0*b1 - a1*b0;        */
            M = sqrt(a*a + b*b + c*c);
            a = a/M;
            b = b/M;
            c = c/M;
            f.plane[0] = a;
            f.plane[1] = b;
            f.plane[2] = c;
            f.plane[3] = -1*(a*x0 + b*y0 + c*z0);				/* -1*(a*x + b*y + c*z); */
            faces.push_back(f);
            break;
        default:          /* invalid commands */
            fgets(buf, sizeof(buf), file);
            fprintf(stderr, "Parse() failed: invalid attributes: \"%c\".\n", ch);
            system("PAUSE");
            exit(-2);
        }
    }
}
//��ʼ��Vertices��ÿ�������Q����
//ǰ�᣺���еĶ��㼯���漯��Ϊ��
//�������QMatrices�ֶ�ȫ����ʼ��
void Quadrics::initQMatrices() {
    if(vertices.size() == 0 || faces.size() == 0) {
        printf("initQMatrices error!\n");
        return;
    }

    //��ʼ��QMatrices�����еľ���Ϊ0����
    //����map�ж��������Ǵ�1��ʼ�������QMatrices��mapҲӦ�ô�1��ʼ��Ҫ
    //һһ��Ӧ����
    for(int i = 1; i <= static_cast<int>(vertices.size()); i++) {
        //���±�ķ�ʽÿ�ζ�������ֵ��ʼ��ȥ��ʼ��value����insert�򲻻�
        //���Ч��
        QMatrices.insert(Matrices::value_type(i, Matrix(0.0)));
    }

    for(int i = 0; i < static_cast<int>(faces.size()); i++) {
        for(int j = 0; j < 3; j++) {
            QMatrices[faces[i].idVertex[j]] += Matrix(faces[i].plane);
        }
    }
}

//��֪pair��idVi��idVj�������error�������Ժ��µĵ�����vx��vy��vz
//Ĭ����������Ϊ0
double Quadrics::calculateErrors(int idVi, int idVj, double* vx, double* vy, double* vz) {
    double pairError;
    Matrix q;
    bool isReturnNewV = true;
    if(vx == NULL && vy == NULL && vz == NULL) {
        vx = new double;
        vy = new double;
        vz = new double;
        isReturnNewV = false;
    }

    q = QMatrices[idVi] + QMatrices[idVj];
    //�ж�q�ǲ��ǶԳ���
    /*if (q[1] != q[4] || q[2] != q[8] || q[6] != q[9] ||
    	q[3] != q[12] || q[7] != q[13] || q[11] != q[14])
    {
    	fprintf(stderr, "ERROR: Matrix q ���ǶԳ���!idVi = %d, idVj = %d\n", idVi, idVj);
    	system("PAUSE");
    	exit(-3);
    }*/
    if(fabs(q[1] - q[4]) > 0.0000001 || fabs(q[2] - q[8]) > 0.0000001 || fabs(q[6] - q[9]) > 0.0000001
            ||
            fabs(q[3] - q[12]) > 0.0000001 || fabs(q[7] - q[13]) > 0.0000001
            || fabs(q[11] - q[14]) > 0.0000001) {
        fprintf(stderr, "ERROR: Matrix q ���ǶԳ���!idVi = %d, idVj = %d\n", idVi, idVj);
        system("PAUSE");
        exit(-3);
    }
    //�ж�error�ķ���ϵ��qDelta�����ǲ��ǿ����
    //����ǿ��������ôֱ�����vx��vy��vz��
    //��չ�����������������error
    Matrix qDelta(q[0], q[1], q[2], q[3],
                  q[4], q[5], q[6], q[7],
                  q[8], q[9], q[10], q[11],
                  0, 0, 0, 1);

    double det;
    // ����
    if(det = qDelta.det(0, 1, 2, 4, 5, 6, 8, 9, 10)) {
        *vx = -1/det*(qDelta.det(1, 2, 3, 5, 6, 7, 9, 10, 11));	/* vx = A41/det(qDelta) */
        *vy =  1/det*(qDelta.det(0, 2, 3, 4, 6, 7, 8, 10, 11));	/* vy = A42/det(qDelta) */
        *vz = -1/det*(qDelta.det(0, 1, 3, 4, 5, 7, 8, 9, 11));	/* vz = A43/det(qDelta) */

        pairError = vertixError(*vx, *vy, *vz, q);
    } else {
        double vx1 = vertices[idVi].x;
        double vy1 = vertices[idVi].y;
        double vz1 = vertices[idVi].z;

        double vx2 = vertices[idVj].x;
        double vy2 = vertices[idVj].y;
        double vz2 = vertices[idVj].z;

        double vx3 = double (vx1+vx2)/2;
        double vy3 = double (vy1+vy2)/2;
        double vz3 = double (vz1+vz2)/2;

        double v1Error = vertixError(vx1, vy1, vz1, q);
        double v2Error = vertixError(vx2, vy2, vz2, q);
        double v3Error = vertixError(vx3, vy3, vz3, q);

        pairError = std::min(std::min(v1Error, v2Error), v3Error);

        if(fabs(v1Error - pairError) <= 0.0000001) {
            *vx = vx1;
            *vy = vy1;
            *vz = vz1;
        }
        if(fabs(v2Error - pairError) <= 0.0000001) {
            *vx = vx2;
            *vy = vy2;
            *vz = vz2;
        }
        if(fabs(v3Error - pairError) <= 0.0000001) {
            *vx = vx3;
            *vy = vy3;
            *vz = vz3;
        }
        /*if(v1Error == pairError) {*vx = vx1; *vy = vy1; *vz = vz1;}
        if(v2Error == pairError) {*vx = vx2; *vy = vy2; *vz = vz2;}
        if(v3Error == pairError) {*vx = vx3; *vy = vy3; *vz = vz3;}*/
    }

    if(isReturnNewV == false) {
        delete vx;
        delete vy;
        delete vz;
    }
    return pairError;
}

//ѡ��Ϸ���pair����ʼ����pair��error��������
void Quadrics::selectPairs() {
    int minId, maxId;

    //�������е��棬ö��v0v1  v0v2  v1v2�������
    //�����pair��error��������Errors
    for(int i = 0; i < static_cast<int>(faces.size()); i++) {
        minId = std::min(faces[i].idVertex[0], faces[i].idVertex[1]);
        maxId = std::max(faces[i].idVertex[0], faces[i].idVertex[1]);
        if(errors.find(Pair(minId, maxId)) == errors.end()) {
            errors.insert(Errors::value_type(Pair(minId, maxId), calculateErrors(minId, maxId)));
        }

        minId = std::min(faces[i].idVertex[0], faces[i].idVertex[2]);
        maxId = std::max(faces[i].idVertex[0], faces[i].idVertex[2]);
        if(errors.find(Pair(minId, maxId)) == errors.end()) {
            errors.insert(Errors::value_type(Pair(minId, maxId), calculateErrors(minId, maxId)));
        }

        minId = std::min(faces[i].idVertex[1], faces[i].idVertex[2]);
        maxId = std::max(faces[i].idVertex[1], faces[i].idVertex[2]);
        if(errors.find(Pair(minId, maxId)) == errors.end()) {
            errors.insert(Errors::value_type(Pair(minId, maxId), calculateErrors(minId, maxId)));
        }
    }
}
//ÿ�δ�errors��ѡ��delta��С��pair�����������������Ҹ����������
void Quadrics::constructContract(int targetFaceNum) {
    Errors::iterator minIter;
    int oriSize = faces.size();
    int processNums = (oriSize - targetFaceNum) / 10;

    while(static_cast<int>(vertices.size()) > targetFaceNum) {
        if((oriSize - faces.size()) % processNums == 0) {
            printf("%d%%\r", ((oriSize - faces.size()) / processNums) * 10);
        }

        minIter = errors.begin();
        double minError = (*minIter).second;
        Pair minPair = (*minIter).first;
        //�ҵ�errors��error��С��pair
        for(; minIter != errors.end(); ++minIter) {
            if(minError > (*minIter).second) {
                minError = ((*minIter).second);
                minPair = (*minIter).first;
            }
        }

        //�����pair�������Ժ�õ���������
        int idVi, idVj;
        idVi = minPair.first;
        idVj = minPair.second;
        //errors.erase(minPair);

        double vx, vy, vz;
        calculateErrors(idVi, idVj, &vx, &vy, &vz);

        //��idVi��ֵ���vx��vy��vz
        vertices[idVi].x = vx;
        vertices[idVi].y = vy;
        vertices[idVi].z = vz;

        //����idVi��Ӧ��q����
        QMatrices[idVi] = QMatrices[idVi] + QMatrices[idVj];
        QMatrices.erase(idVj);
        //����face��Ϣ������idVi idVj�ߵ��棬ֱ��ɾ����
        //����idVjΪ������棬��idVj�滻��idVi
        for(Faces::iterator iter = faces.begin(); iter != faces.end(); ) {
            int j = 0;
            for(; j < 3; j++) {
                if(iter->idVertex[j] == idVj) {
                    if(iter->idVertex[0] == idVi || iter->idVertex[1] == idVi || iter->idVertex[2] == idVi) {
                        iter = faces.erase(iter);
                    } else {
                        iter->idVertex[j] = idVi;
                        ++iter;
                    }
                    break;
                } else if(j == 2)
                    iter++;
            }
        }

        //���¶����
        vertices.erase(idVj);

        //ɾ��errors����صıߣ�����Ҫ���¼���errors�ж�Ӧ��ֵ
        Pair p;
        std::pair<Errors::iterator, bool> pr;
        for(Errors::iterator iter = errors.begin(); iter != errors.end(); ) {
            p = iter->first;
            if(p.first == idVj && p.second != idVi) {
                iter = errors.erase(iter);
                pr = errors.insert(Errors::value_type(Pair(std::min(idVi, p.second), std::max(idVi, p.second)),
                                                      0.0));
            } else if(p.second == idVj && p.first != idVi) {
                iter = errors.erase(iter);
                pr = errors.insert(Errors::value_type(Pair(std::min(idVi, p.first), std::max(idVi, p.first)), 0.0));
            } else {
                ++iter;
            }
        }
        errors.erase(minPair);

        //����errors�е�ֵ,���ж�����а���idVi�Ķ�Ҫ���¼���
        for(Errors::iterator iter = errors.begin(); iter != errors.end(); ++iter) {
            p = iter->first;
            if(p.first == idVi) {
                iter->second = calculateErrors(p.first, p.second);
            }
            if(p.second == idVi) {
                iter->second = calculateErrors(p.first, p.second);
            }
        }
    }
}

void Quadrics::write_smf(const char* filename) {
    FILE *file;
    fopen_s(&file, filename, "w");
    if (file == NULL) {
        fprintf(stderr, "write_smf() failed: can't write data file \"%s\".\n", filename);
        system("PAUSE");
        exit(-1);
    }

    /* print header info */
    fprintf(file, "#$PM 0.1\n");
    fprintf(file, "#$vertices %d\n", vertices.size());
    fprintf(file, "#$faces %d\n", faces.size());
    fprintf(file, "#\n");

    /* print vertices */
    int ii = 1;
    std::map<int, int> imap;
    for (Vertices::iterator iter = vertices.begin(); iter != vertices.end(); iter++) {
        fprintf(file, "v %lf %lf %lf\n", /*iter->first, */iter->second.x, iter->second.y, iter->second.z);
        imap.insert(std::map<int, int>::value_type(iter->first, ii));
        ii++;
    }
    int v1, v2, v3;
    for (int i = 0; i < static_cast<int>(faces.size()); i++) {
        v1 = imap[faces[i].idVertex[0]];
        v2 = imap[faces[i].idVertex[1]];
        v3 = imap[faces[i].idVertex[2]];
        fprintf(file, "f %d %d %d\n", v1, v2, v3);
    }

    fclose(file);
}

// ����һ���������꣬������������������ƽ�����ƽ������Сֵ
double Quadrics::minDisSquaredToAllPlance(Vertex vertex) {

    double minSquaredDistance = FLT_MAX;

    double squaredDistance = 0.0;
    double* plane = NULL;
    for(Faces::iterator faceIter = faces.begin(); faceIter != faces.end(); ++faceIter) {
        plane = faceIter->plane;
        squaredDistance = plane[0] * vertex.x + plane[1] * vertex.y + plane[2] * vertex.z + plane[3];
        squaredDistance *= squaredDistance;

        if(minSquaredDistance > squaredDistance) {
            minSquaredDistance = squaredDistance;
        }
    }

    return minSquaredDistance;
}


// ������Ժ��ԭʼ��������ƶ�
double Quadrics::calculateSimilarity(Quadrics& originQuadrics) {
    //�����㣬�򻯺�����Ĳ����㱾��ԭʼ����Ĳ��������
    //�򻯺������еĶ������

    Vertices originVertices = originQuadrics.getVertices();
    Vertices simpledVertices = vertices;
    //2.����ԭʼ��������㵽�򻯺�����������ƽ�����С����ƽ���ͣ�ͬʱ�ۼ�
    double squaredDisFromOrigin = 0.0;
    //����ԭʼ���������еĵ㣬���ռ򻯺������е��index��ȡ��Ӧ�ĵ㣬ͬʱ����㵽�򻯺�
    //�������̾���ƽ����
    for(Vertices::iterator iter = simpledVertices.begin(); iter != simpledVertices.end(); ++iter) {
        int index = iter->first;
        Vertices::iterator originVertice = originVertices.find(index);
        if(originVertice != originVertices.end()) {
            //����õ㵽�򻯺�����������ƽ�����С����ƽ����
            squaredDisFromOrigin += minDisSquaredToAllPlance(originVertice->second);
        } else {
            fprintf(stderr, "������ԭʼ�������ҵ�%d����\n", index);
            system("PAUSE");
            exit(-1);
        }
    }
    printf("squaredDisFromOrigin:%e\n", squaredDisFromOrigin);
    //3.����򻯺������еĶ��㵽ԭʼ��������ƽ�����С����ƽ���ͣ�ͬʱ�ۼ�
    double squareDisFromSimpled = 0.0;
    for(Vertices::iterator iter = simpledVertices.begin(); iter != simpledVertices.end(); ++iter) {
        //����õ㵽ԭʼ����������ƽ�����С����ƽ����
        squareDisFromSimpled += originQuadrics.minDisSquaredToAllPlance(iter->second);
    }
    printf("squareDisFromSimpled:%e\n", squareDisFromSimpled);
    return (squaredDisFromOrigin + squareDisFromSimpled) / (simpledVertices.size() * 2);
}

