#include "reconstruct.h"

void Reconstruction::write(std::string path, FILE *fp, bool prunePath) {
	if (!perm.empty()) {
		if (prunePath) {
			std::size_t found = path.rfind("/");
			path = (path.substr(found+1)).c_str();
		}

		fprintf(fp, "%s\n", path.c_str());
		for (auto x : perm) {
			fprintf(fp, "%d ", x);
		}
		fprintf(fp, "\n");
	}
}

void Reconstruction::write(FILE *fp, bool prunePath) {
	write(*source, fp, prunePath);
}

Reconstruction unshuffleImage(std::string path) {
    std::unique_ptr<Estimate> e(new Estimate(path));
    std::vector<int> perm = e->result;

    std::set<int> missing;
    for (int i = 0; i < NUM_PIECES; ++i) {
        missing.insert(i);
    }

    for (auto x : perm) {
        if (x >= 0) {
            missing.erase(x);
        }
    }
    std::vector<int> Z;
    std::copy(missing.begin(), missing.end(), std::back_inserter(Z));

    random_shuffle(Z.begin(), Z.end());

    int c = 0;
    for (auto &x : perm) {
        if (x < 0) {
            x = Z[c];
            ++c;
        }
    }

    return Reconstruction(perm, path);
}
