#include <Model.h>

void Model::load_from_obj(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp) {
		std::cout << "Loading failed." << std::endl;
	}
	else {
		char buf[256];
		int nVertices = 0, nTriangles = 0;
		int nLines = 0;
		while (fscanf(fp, "%s", buf) != EOF) {
			nLines++;
			switch (buf[0]) {
			case '#':
				fgets(buf, sizeof(buf), fp);
				break;
			case 'v':
				switch (buf[1]) {
				case '\0':
				{
					double x, y, z;
					if (fscanf(fp, "%lf %lf %lf", &x, &y, &z) == 3) {
						vertices.push_back(x);
						vertices.push_back(y);
						vertices.push_back(z);
					}
					else {
						std::cout << "Wrong number of values." << std::endl;
					}
				}
				break;
				default:
					fgets(buf, sizeof(buf), fp);
					break;
				}
				break;
			case 'f':
			{
				GLuint v, n, t;
				GLuint a, b, c;
				if (fscanf(fp, "%s", buf) != 1) {
					std::cout << "Wrong Face!" << std::endl;
				}
				if (strstr(buf, "//")) {
					if (sscanf(buf, "%d//%d", &a, &n) == 2 &&
						fscanf(fp, "%d//%d", &b, &n) == 2 &&
						fscanf(fp, "%d//%d", &c, &n) == 2) {
						nTriangles++;
						indices.push_back(a - 1);
						indices.push_back(b - 1);
						indices.push_back(c - 1);
					}
					else {
						std::cout << "Wrong!" << std::endl;
					}
				}
				else {
					if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
						a = v;
						if (fscanf(fp, "%d/%d/%d", &b, &t, &n) == 3 &&
							fscanf(fp, "%d/%d/%d", &c, &t, &n) == 3)
						{
							nTriangles++;
							indices.push_back(a - 1);
							indices.push_back(b - 1);
							indices.push_back(c - 1);
						}
						else {
							std::cout << "wrong face~" << std::endl;
						}
					}
					else {
						if (sscanf(buf, "%d", &a) == 1 &&
							fscanf(fp, "%d", &b) == 1 &&
							fscanf(fp, "%d", &c) == 1) {
							nTriangles++;
							indices.push_back(a - 1);
							indices.push_back(b - 1);
							indices.push_back(c - 1);
						}
						else {
							std::cout << "wrong face~~" << std::endl;
						}
					}
				}
			}
			break;
			default:
				fgets(buf, sizeof(buf), fp);
				break;
			}
		}
	}
	fclose(fp);
	ori_indicies = indices;
	ori_vertices = vertices;
}

void Model::calulateQ(int index)
{
	glm::mat4 Qmat(0);
	glm::vec3 A, B, C;
	getPoint(index, A);
	for (pii tri : G[index]) {
		getPoint(tri.first, B);
		getPoint(tri.second, C);
		glm::vec3 N = glm::cross(A - B, A - C);
		N = glm::normalize(N);
		float d = -glm::dot(N, A);
		glm::vec4 tmp(N, d);
		/*Qmat = Qmat + glm::mat4(N.x*N.x, N.x*N.y, N.x*N.z, N.x*d,
			N.y*N.x, N.y*N.y, N.y*N.z, N.y*d,
			N.z*N.x, N.z*N.y, N.z*N.z, N.z*d,
			d*N.x, d*N.y, d*N.z, d*d);*/
		Qmat = Qmat + glm::outerProduct(tmp,tmp);
	}
	Q[index] = Qmat;
}

void Model::updateHeap(int index)
{
	for (std::set<int>::iterator it = E[index].begin(); it != E[index].end(); it++) {
		int j = *it;
		glm::mat4 Q_bar = Q[index] + Q[j];
		glm::vec3 a, b;
		getPoint(index, a);
		getPoint(j, b);
		glm::vec4 A(a, 1.0f), B(b, 1.0f);
		glm::vec4 C = (A + B) * 0.5f;
		// point[i], point[j], (point[i]+point[j]) / 2
		// 1. point[i]
		float err;
		err = glm::dot(A,(Q_bar * A));
		EdgePair ep1 = EdgePair{ index,j,err,A };
		err = glm::dot(B,(Q_bar * B));
		EdgePair ep2 = EdgePair{ index,j,err,B };
		err = glm::dot(C,(Q_bar * C));
		EdgePair ep3 = EdgePair{ index,j,err,C };
		EdgePair ep = std::min(ep1, ep2);
		ep = std::min(ep, ep3);
		heap.push(ep);
	}
}

void Model::simplify(float ratio)
{
	vertices = ori_vertices;
	indices = ori_indicies;
	makeG(indices);
	makeE(indices);
	Exist.clear();
	for (auto it = indices.begin(); it != indices.end(); it++) Exist.insert(*it);
	for (int i = 0; i < vertices.size() / 3; i++)
	{
		if (Exist.find(i) == Exist.end()) continue;
		calulateQ(i);
	}
	while (!heap.empty()) heap.pop();
	for (int i = 0; i < vertices.size() / 3; i++) {
		if (Exist.find(i) == Exist.end()) continue;
		updateHeap(i);
	}

	int tot = ratio * indices.size() / 6;
	int _tot = tot;
	while (tot && !heap.empty()) {
		EdgePair ep = heap.top();
		heap.pop();
		if (Exist.find(ep.a) != Exist.end() && Exist.find(ep.b) != Exist.end()) {
			tot -= 1;
			Exist.erase(ep.a); Exist.erase(ep.b);
			auto p = ep.point;
			vertices.push_back(p.x); vertices.push_back(p.y); vertices.push_back(p.z);
			int ind = vertices.size() / 3 - 1;
			Exist.insert(ind);
			// union edge
			std::set<int> unionset;
			for (auto it = E[ep.a].begin(); it != E[ep.a].end(); it++) {
				if (Exist.find(*it) == Exist.end()) continue;
				E[*it].erase(ep.a);
				E[*it].insert(ind);
				unionset.insert(*it);
			}
			for (auto it = E[ep.b].begin(); it != E[ep.b].end(); it++) {
				if (Exist.find(*it) == Exist.end()) continue;
				E[*it].erase(ep.b);
				E[*it].insert(ind);
				unionset.insert(*it);
			}
			E.push_back(unionset);
			E[ep.a].clear();
			E[ep.b].clear();
			// union G
			sp unionG;
			for (auto it = G[ep.a].begin(); it != G[ep.a].end(); it++) {
				if (Exist.find(it->first) == Exist.end()) continue;
				if (Exist.find(it->second) == Exist.end()) continue;
				unionG.insert(*it);
				if (G[it->first].find(std::make_pair(ep.a, it->second)) != G[it->first].end()) {
					G[it->first].erase(std::make_pair(ep.a, it->second));
					G[it->first].insert(std::make_pair(ind, it->second));
				}
				if (G[it->first].find(std::make_pair(it->second, ep.a)) != G[it->first].end()) {
					G[it->first].erase(std::make_pair(it->second, ep.a));
					G[it->first].insert(std::make_pair(it->second, ind));
				}
				//G[it->first].insert(std::make_pair(ind, it->second));
				if (G[it->second].find(std::make_pair(ep.a, it->first)) != G[it->second].end()) {
					G[it->second].erase(std::make_pair(ep.a, it->first));
					G[it->second].insert(std::make_pair(ind, it->first));
				}
				if (G[it->second].find(std::make_pair(it->first, ep.a)) != G[it->second].end()) {
					G[it->second].erase(std::make_pair(it->first, ep.a));
					G[it->second].erase(std::make_pair(it->first, ind));
				}
				//G[it->second].insert(std::make_pair(ind, it->first));
			}
			for (auto it = G[ep.b].begin(); it != G[ep.b].end(); it++) {
				if (Exist.find(it->first) == Exist.end()) continue;
				if (Exist.find(it->second) == Exist.end()) continue;
				unionG.insert(*it);
				if (G[it->first].find(std::make_pair(ep.b, it->second)) != G[it->first].end()) {
					G[it->first].erase(std::make_pair(ep.b, it->second));
					G[it->first].insert(std::make_pair(ind, it->second));
				}
				if (G[it->first].find(std::make_pair(it->second, ep.b)) != G[it->first].end()) {
					G[it->first].erase(std::make_pair(it->second, ep.b));
					G[it->first].insert(std::make_pair(it->second, ind));
				}
				//G[it->first].insert(std::make_pair(ind, it->second));
				if (G[it->second].find(std::make_pair(ep.b, it->first)) != G[it->second].end()) {
					G[it->second].erase(std::make_pair(ep.b, it->first));
					G[it->second].insert(std::make_pair(ind, it->first));
				}
				if (G[it->second].find(std::make_pair(it->first, ep.b)) != G[it->second].end()) {
					G[it->second].erase(std::make_pair(it->first, ep.b));
					G[it->second].insert(std::make_pair(it->first, ind));
				}
				//G[it->second].insert(std::make_pair(ind, it->first));
			}
			G.push_back(unionG);
			G[ep.a].clear();
			G[ep.b].clear();
			// calulateQ
			calulateQ(ind);
			//updateHeap
			updateHeap(ind);
		}
	}
	// update indices
	std::cout << "Face: " << indices.size() / 3 << std::endl;
	indices.clear();
	std::set<Face> faces;
	for (int i = 0; i < vertices.size() / 3; i++) {
		for (auto tri : G[i]) {
			if (Exist.find(i) == Exist.end()) continue;
			if (Exist.find(tri.first) == Exist.end()) continue;
			if (Exist.find(tri.second) == Exist.end()) continue;
			if (faces.find(Face(i, tri.first, tri.second)) == faces.end()) {
				indices.push_back(i);
				indices.push_back(tri.first);
				indices.push_back(tri.second);
				faces.insert(Face(i, tri.first, tri.second));
			}
		}
	}
	
	std::cout << "Face: " << indices.size() / 3 << std::endl;
}

void Model::save(const char * filename)
{
	FILE *fp = fopen(filename, "w");
	for (int i = 0; i < vertices.size(); i += 3)
	{
		fprintf(fp, "v %f %f %f\n", vertices[i], vertices[i + 1], vertices[i + 2]);
	}
	for (int i = 0; i < indices.size(); i += 3) {
		fprintf(fp, "f %d %d %d\n", indices[i] + 1, indices[i + 1] + 1, indices[i + 2] + 1);
	}
	fclose(fp);
}

void Model::makeG(std::vector<unsigned int> indices)
{
	G.clear();
	G.resize(vertices.size() / 3);
	for (int i = 0; i < indices.size(); i+=3) {
		int a = indices[i], b = indices[i + 1], c = indices[i + 2];
		G[a].insert(std::make_pair(b, c));
		G[b].insert(std::make_pair(c, a));
		G[c].insert(std::make_pair(a, b));
	}
}

void Model::makeE(std::vector<unsigned int> indices)
{
	E.clear();
	E.resize(vertices.size() / 3);
	for (int i = 0; i < indices.size(); i += 3) {
		int a = indices[i], b = indices[i + 1], c = indices[i + 2];
		E[a].insert(b); E[a].insert(c);
		E[b].insert(a); E[b].insert(c);
		E[c].insert(a); E[c].insert(b);
	}
}
