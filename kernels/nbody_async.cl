constant int timestep = 86400;///24*60*60, seconds in a day
constant float G = -6.67e-11; //graviational constant

/*
General explanation:
The local memory size is bounded (e.g. 48kB on RTX 3070)
Each body uses 16 Bytes (3 floats for position, 1 for mass)
With that the local memory can fit 3000 bodies
If nrBodies > 3000, then we cannot fit all bodies into local memory at the same time.

Therefore we have to load the bodies into local in different runs, 
for 9000 Bodies, one would need 3 runs to load all bodies atleast once, for 9001 bodies, 4 runs are needed.

Each body has to calculate distance to each other body and read its mass, which is why
putting them into local memory should reduce the time needed for each run.
(Interestingly, it doesn't reduce calculation time)

Further notes:
In the optimal case, all work items read one body and load it into local memory for all others to see,
but neither the work group size nor the local memory size normally allow that.

Comparison with kernel in "nbody_local.cl":
The only differnce is in the way the bodies are read into local memory
This kernel uses async copy from OpenCL while the _local kernel tries to copy
each body with a way more complicated logic.
Besides the copy mechanism, they are equivalent.
*/
__kernel void nbody_force_calculation(global read_write float *positions,
                                      global read_write float *velocities,
                                      global read_only float *masses,
                                      read_only int nrBodies,
                                      read_only int bodiesPerRun,
                                      read_only int maxNrBodiesInLocal,
                                      local float *l_pos,
                                      local float *l_mass) {
    const size_t g_id = get_global_id(0);
    const size_t gSize = get_global_size(0);
    const int id3 = g_id * 3;
    const int id31 = id3 + 1;
    const int id32 = id3 + 2;

    // we have more work items than we have bodies (so they can be evenly split)
    // so some work items don't calculate anything
    // but these items also have to run into the barriers, so we don't deadlock
    const bool calc = g_id < nrBodies;


    float4 acceleration = (float4) (0, 0, 0, 0);
    float4 bodyPos;
    if (calc) {
        // each work item calculates values for one body only
        bodyPos = (float4) (positions[id3], positions[id31], positions[id32], 0);
    }
    int runIndex;

    const int runs_needed = ceil(native_divide((float) gSize, (float) maxNrBodiesInLocal));
    // because we only calcuate on bodies in local memory, each workgroup needs to load
    // a split part of the bodies into local memory, where each item loads a fixed amount of bodies (bodies per cycle)
    // into local mem (more bodies fit into local memory than we have work items in a workgroup

    for (int run = 0; run < runs_needed; run++) {
        runIndex = maxNrBodiesInLocal * run;
        int bodiesToProcess = min(nrBodies - runIndex, maxNrBodiesInLocal);

        // loads memory from global to local of size bodiesToProcess (*3)
        event_t posEvt;
        posEvt = async_work_group_copy(l_pos, &positions[runIndex * 3], bodiesToProcess * 3, posEvt);

        event_t massEvt;
        massEvt = async_work_group_copy(l_mass, &masses[runIndex], bodiesToProcess, massEvt);

        // waits for copy to finish
        event_t events[2] = {posEvt, massEvt};
        wait_group_events(2, &events);

        //for each body in local memory
        if (calc) {
            for (uint i = 0; i < bodiesToProcess; i++) {
                // calculate force for each body
                // continue if current body to calculate is own body
                if (runIndex + i == g_id) {
                    continue;
                }
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
