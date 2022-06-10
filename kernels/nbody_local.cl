constant int timestep = 86400;///24*60*60, seconds in a day

/*
For documentation see file nbody_async.cl
The only difference here is the copy mechanism, 
which is the only thing commented here.
*/
__kernel void nbody_force_calculation(global read_write float *positions, global read_write float *velocities, global read_only float *masses,
                                      read_only int nrBodies, read_only int bodiesPerRun, read_only int maxNrBodiesInLocal,
                                      local float *l_pos, local float *l_mass) {
    const size_t g_id = get_global_id(0);
    const size_t gSize = get_global_size(0);
    const int id3 = g_id * 3;
    const int id31 = id3 + 1;
    const int id32 = id3 + 2;
    const size_t l_id = get_local_id(0);
    const bool calc = g_id < nrBodies;


    float4 acceleration = (float4) (0, 0, 0, 0);
    float4 bodyPos;
    if (calc) {
        bodyPos = (float4) (positions[id3], positions[id31], positions[id32], 0);
    }
    int runIndex;

    const int runs_needed = ceil(((float) gSize) / maxNrBodiesInLocal);
    int locIndex;
    int globIndex;
    for (int run = 0; run < runs_needed; run++) {
        runIndex = maxNrBodiesInLocal * run;
        int bodiesToProcess = min(nrBodies - runIndex, maxNrBodiesInLocal);

        // because the amount of bodies which can fit into local memory
        // is normally larger than the amount of work items in a group
        // each work item has to copy multiple items (bodiesPerRun - many)
        // into local memory
        for (int bpc = 0; bpc < bodiesPerRun; bpc++) {
            locIndex = l_id * bodiesPerRun + bpc;
            globIndex = (runIndex + locIndex) * 3;
            if (locIndex >= bodiesToProcess) {
                break;
            }
            l_mass[locIndex] = masses[runIndex + locIndex];

            l_pos[locIndex * 3] = positions[globIndex];
            l_pos[locIndex * 3 + 1] = positions[globIndex + 1];
            l_pos[locIndex * 3 + 2] = positions[globIndex + 2];
        }

        //done writing to local memory
        barrier(CLK_LOCAL_MEM_FENCE);
        if (calc) {
            for (uint i = 0; i < bodiesToProcess; i++) {
                if (runIndex + i == g_id) {
                    continue;
                }
                // calculate distance between
                float4 otherPos = (float4) (l_pos[i * 3], l_pos[i * 3 + 1], l_pos[i * 3 + 2], 0);
                float4 distance = bodyPos - otherPos;
                float sq = dot(distance, distance);
                acceleration += distance * native_rsqrt(sq) * native_divide(l_mass[i], sq);
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    if (calc) {
        acceleration *= timestep;
        velocities[id3] += acceleration.x;
        velocities[id31] += acceleration.y;
        velocities[id32] += acceleration.z;
    }
}