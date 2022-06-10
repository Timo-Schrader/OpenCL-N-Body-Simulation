kernel void nbody_force_calculation(global float *positions, global float *velocities, global float *masses, int nrBodies) {
    const int timestep = 86400;///24*60*60, seconds in a day
    const int id = get_global_id(0);
    const int id3 = id * 3;
    const int id31 = id3 + 1;
    const int id32 = id3 + 2;//saves 2 computations this way
    float acc1 = 0;
    float acc2 = 0;
    float acc3 = 0;

    if (id < nrBodies) {
        float cPos1 = positions[id3];
        float cPos2 = positions[id31];
        float cPos3 = positions[id32];

        // for each body
        for (int i = 0; i < nrBodies; i++) {
            //calculate force for each body

            if (i == id) {
                continue;
            }
            // distance without square root
            float dis1 = cPos1 - positions[i * 3];
            float dis2 = cPos2 - positions[i * 3 + 1];
            float dis3 = cPos3 - positions[i * 3 + 2];
            // the squared distance is needed twice
            // otherwise we have to square the sqrt again
            float sq = dis1 * dis1 + dis2 * dis2 + dis3 * dis3;
            // using native methods for massive performance boost
            // rsqrt is inverse sqrt 1/sqrt(x)
            // masses have already been precalculated with G=6.7*10^(-11)
            // so that they don't have to be calculated multiple times
            float temp = native_divide(masses[i], sq) * native_rsqrt(sq);
            //calculates the acceleration for the x y z and sums them up
            acc1 += dis1 * temp;
            acc2 += dis2 * temp;
            acc3 += dis3 * temp;
        }

        velocities[id3] += acc1 * timestep;
        velocities[id31] += acc2 * timestep;
        velocities[id32] += acc3 * timestep;
    }
}
