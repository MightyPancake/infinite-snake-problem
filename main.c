#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "arr.h" //My implementation of dynamic arrays

int min(int a, int b){
    if (a > b) return b;
    else return a;
}

int max(int a, int b){
    if (a < b) return b;
    else return a;
}

//n^2
bool is_sol_valid(Array* points, Array* obstacles, Array* sol){
    int o = obstacles->len;
    int n = sol->len;
    if (sol->len < points->len) return false;
    Array* obs;
    Vector2 p1, p2, p3, p4, p5;
    for (int i = 0; (i+1)<n; i++){
        p1 = arr_at(Vector2, points, arr_at(int, sol, i));
        p2 = arr_at(Vector2, points, arr_at(int, sol, (i+1)));
        for (int j = 0; (j+1)<n; j++){
            if ((i==j || i==j+1 || i+1==j)) continue;
            p3 = arr_at(Vector2, points, arr_at(int, sol, j));
            p4 = arr_at(Vector2, points, arr_at(int, sol, (j+1)));
            if (CheckCollisionLines(p1, p2, p3, p4, NULL)){
                return false;
            }
        }
        for (int j = 0; j<o; j++){
            obs = arr_at(Array*, obstacles, j);
            p3 = arr_at(Vector2, obs, 0);
            p4 = arr_at(Vector2, obs, 1);
            p5 = arr_at(Vector2, obs, 2);
            if (CheckCollisionLines(p1, p2, p3, p4, NULL) || CheckCollisionLines(p1, p2, p4, p5, NULL) || CheckCollisionLines(p1, p2, p3, p5, NULL)){
                return false;
            }
        }
    }
    return true;
}

Array* only_valid_ans(Array* ans, Array* points, Array* obstacles){
    int valid_c = 0;
    int invalid_c = 0;
    int n = points->len;
    arr_index ans_c = ans->len;
    Array* sol;
    Array* valid_ans = new_arr();
    //valid_ans->len<(n/2) && 
    while(ans->len > 0){
        sol = arr_pop(Array*, ans);
        if (is_sol_valid(points, obstacles, sol)){
            arr_push(Array*, valid_ans, sol);
            //valid_c++;
        }else{
            free_arr(sol);
            //invalid_c++;
        }
    }
    while(ans->len>0){
        sol = arr_pop(Array*, ans);
        free_arr(sol);
    }
    free_arr(ans);
    //printf("Generated %d valid and %d invalid solutions out of %lld\n", valid_c, invalid_c, ans_c);
    return valid_ans;
}

Array* del_duplicates(Array* ans, int n){
    Array* new_ans = new_arr();
    for (int i = 0; i<n; i++) arr_push(Array*, new_ans, new_arr());
    Array* done = new_arr_len(bool, n);
    Array* sol;
    bool sol_used;
    int start, end;
    int satisfied = 0;
    while(ans->len>0 && satisfied < n){
        sol = arr_pop(Array*, ans);
        if (sol->len < n){
            free_arr(sol);
            continue;
        }
        sol_used = false;
        start = arr_first(int, sol);
        end = arr_last(int, sol);
        if (!arr_at(bool, done, start)){
            arr_set(bool, done, start, true);
            arr_set(Array*, new_ans, start, sol);
            sol_used = true;
            satisfied++;
        }
        if (!arr_at(bool, done, end)){
            arr_set(bool, done, end, true);
            arr_set(Array*, new_ans, end, sol);
            sol_used = true;
            satisfied++;
        }
        if (!sol_used){
            free_arr(sol);
        }
    }
    printf("BRUTE FORCE result:\n%d/%d satisfied!\n", satisfied, n);
    return new_ans;
}

Array* brute_force(Array* points, Array* obstacles){
    int n = points->len;
    Array* ans = new_arr();
    for (int i = 0; i<n; i++)
        arr_push(Array*, ans, new_arr());
    Array* combs = new_arr();
    Array* new_combs = new_arr();
    Array* sol;
    Array* pts = new_arr();
    for (int i = 0; i<n; i++){
        arr_push(int, pts, i);
    }
    for (int i = 0; i<n; i++){
        Array* st = new_arr_len(Array*, 2);
        Array* used = new_arr();
        Array* unused = arr_cpy(int, pts);
        arr_push(int, used, i);
        arr_del(int, unused, i);
        arr_push(Array*, st, used);
        arr_push(Array*, st, unused);
        arr_push(Array*, combs, st);
    }
    Array* comb;
    Array* n_used;
    Array* n_unused;
    while(combs->len > 0){
        for (arr_index i = 0; i<combs->len; i++){
            comb = arr_at(Array*, combs, i);
            Array* unused = arr_pop(Array*, comb);
            Array* used = arr_pop(Array*, comb);
            free_arr(comb);
            if (unused->len == 0){
                arr_push(Array*, ans, used);
                free_arr(unused);
                continue;
            }
            for (arr_index j = 0; j<unused->len; j++){
                Array* new_comb = new_arr();
                n_used =  arr_cpy(Array*, used);
                n_unused = arr_cpy(Array*, unused);
                arr_push(int, n_used, arr_at(int, unused, j));
                arr_del(int, n_unused, j);
                arr_push(Array*, new_comb, n_used);
                arr_push(Array*, new_comb, n_unused);
                arr_push(Array*, new_combs, new_comb);
            }
            free_arr(used);
            free_arr(unused);
        }
        free_arr(combs);
        combs = new_combs;
        new_combs = new_arr();
    }
    free_arr(combs);
    free_arr(new_combs);
    free_arr(pts);
    //Filter the answer
    ans = only_valid_ans(ans, points, obstacles);
    ans = del_duplicates(ans, n);
    return ans;
}

bool is_lastly_added_ok(Array* points, Array* obstacles, Array* sol){ //O(n) instead of O(n^2), only checks last addition ofc.
    int n = sol->len;
    int o = obstacles->len;
    if (n<=1) return true;
    Array* obs;
    Vector2 p1, p2, p3, p4, p5;
    p1 = arr_at(Vector2, points, arr_at(int, sol, n-2));
    p2 = arr_at(Vector2, points, arr_at(int, sol, n-1));
    for (int i = 0; (i+1)<(n-2); i++){
        p3 = arr_at(Vector2, points, arr_at(int, sol, i));
        p4 = arr_at(Vector2, points, arr_at(int, sol, (i+1)));
        if (CheckCollisionLines(p1, p2, p3, p4, NULL)){
            return false;
        }
    }
    for (int i = 0; i<o; i++){
        obs = arr_at(Array*, obstacles, i);
        p3 = arr_at(Vector2, obs, 0);
        p4 = arr_at(Vector2, obs, 1);
        p5 = arr_at(Vector2, obs, 2);
        if (CheckCollisionLines(p1, p2, p3, p4, NULL) || CheckCollisionLines(p1, p2, p4, p5, NULL) || CheckCollisionLines(p1, p2, p3, p5, NULL)){
            return false;
        }
    }
    return true;
}

Array* bfs_brute_rec(Array* points, Array* obstacles, Array* st){
    int n = points->len;
    Array* sol;
    Array* unused = arr_pop(Array*, st);
    Array* used = arr_pop(Array*, st);
    if (!is_lastly_added_ok(points, obstacles, used)){
        return new_arr();
    }
    if (unused->len == 0){
        sol = arr_cpy(int, used);
        if (is_sol_valid(points, obstacles, sol)){
            return sol;
        }else{
            free_arr(sol);
            return new_arr();
        }
    }else{
        Array* n_used;
        Array* n_unused;
        Array* n_st;
        for (int i = 0; i<unused->len; i++){
            n_st = new_arr();
            n_used = arr_cpy(int, used);
            n_unused = arr_cpy(int, unused);
            int p = arr_at(int, n_unused, i);
            arr_push(int, n_used, p);
            arr_del(int, n_unused, i);
            arr_push(Array*, n_st, n_used);
            arr_push(Array*, n_st, n_unused);
            sol = bfs_brute_rec(points, obstacles, n_st);
            free_arr(n_st);
            if (sol->len > 0){
                return sol;
            }else{
                free_arr(sol);
                //free_arr(n_used);
                //free_arr(n_unused);
            }
        }
        return new_arr();
    }
}

Array* brute_dfs(Array* points, Array* obstacles){
    //Inspect latest added answer, skip duplicates. Should be faster by A LOT than brute_force
    Array* sol;
    Array* st;
    Array* used;
    Array* unused;
    int n = points->len;
    Array* done = new_arr_len(bool, n);
    Array* ans = new_arr();
    for (int i = 0; i<n; i++)
        arr_push(Array*, ans, new_arr());
    int satisfied = 0;
    Array* pts = new_arr();
    for (int i = 0; i<n; i++){
        arr_push(int, pts, i);
    }
    for (int i = 0; i<n; i++){
        if (arr_at(bool, done, i)) continue;
        st = new_arr();
        used = new_arr();
        arr_push(int, used, i);
        unused = arr_cpy(int, pts);
        arr_del(int, unused, i);
        arr_push(Array*, st, used);
        arr_push(Array*, st, unused);
        sol = bfs_brute_rec(points, obstacles, st);
        if (sol->len > 0){
            int lst = arr_last(int, sol);
            if(!arr_at(bool, done, i)){
                arr_set(Array*, ans, i, sol);
                arr_set(bool, done, i, true);
            }
            if(!arr_at(bool, done, lst)){
                arr_set(Array*, ans, lst, sol);
                arr_set(bool, done, lst, true);
            }
        }else{
            printf("BRUTE DFS failed to find any answer for point #%d\n", i+1);
        }
        satisfied = 0;
        for (int j = 0; j<n; j++){
            if (arr_at(bool, done, j))  satisfied++;
        }
        printf("BRUTE DFS result:\nSatisfied %d/%d\n", satisfied, n);
        free_arr(st);
        free_arr(unused);
        free_arr(used);
    }
    return ans;
}

Array* get_line_score(Array* points, Array* obstacles){
    int n = points->len;
    int o = obstacles->len;
    int ind1, ind2;
    Vector2 p1, p2, p3, p4, p5;
    Array* obs;
    Array* score = new_arr_len(int, n*n);
    Array* lin1;
    Vector2 line1[2];
    Vector2 line2[2];
    for(arr_index i = 0; i<points->len; i++){
        line1[0] = arr_at(Vector2, points, i);
        for(arr_index j = 0; j<points->len; j++){
            if (i==j) continue;
            line1[1] = arr_at(Vector2, points, j);
            ind1 = max(i, j)+n*min(i, j);
            for(arr_index k = 0; k<points->len; k++){
                line2[0] = arr_at(Vector2, points, k);
                for(arr_index l = 0; l<points->len; l++){
                    if (k==l) continue;
                    line2[1] = arr_at(Vector2, points, l);
                    ind2 = max(k, l)+n*min(k, l);
                    if (ind1 == ind2) continue; //Same lines, shouldn't collide!
                    if (CheckCollisionLines(line1[0], line1[1], line2[0], line2[1], NULL)){
                        //arr_push(int, lin1, max(k, l)+n*min(k, l));
                        arr_set(int, score, ind1, arr_at(int, score, ind1) + 1);
                        arr_set(int, score, ind2, arr_at(int, score, ind2) + 1);
                    }
                }
            }
            p1 = line1[0];
            p2 = line1[1];
            for (int k = 0; k<o; k++){
                obs = arr_at(Array*, obstacles, k);
                p3 = arr_at(Vector2, obs, 0);
                p4 = arr_at(Vector2, obs, 1);
                p5 = arr_at(Vector2, obs, 2);
                if (CheckCollisionLines(p1, p2, p3, p4, NULL) || CheckCollisionLines(p1, p2, p4, p5, NULL) || CheckCollisionLines(p1, p2, p3, p5, NULL)){
                    arr_set(int, score, ind1, arr_at(int, score, ind1) + 3);
                }
            }
        }
    }
    return score;
}

bool points_outside_obstacles(Array* points, Array* obstacles){
    int n = points->len;
    int o = obstacles->len;
    Array* obs;
    Vector2 p, p1, p2, p3;
    for (int i = 0; i<n; i++){
        p = arr_at(Vector2, points, i);
        for (int j = 0; j<o; j++){
            obs = arr_at(Array*, obstacles, j);
            p1 = arr_at(Vector2, obs, 0);
            p2 = arr_at(Vector2, obs, 1);
            p3 = arr_at(Vector2, obs, 2);
            if (CheckCollisionPointTriangle(p, p1, p2, p3))
                return false;
        }
    }
    return true;
}

Array* best_lines_rec(Array* points, Array* obstacles, Array* st, Array* ls){
    int n = points->len;
    Array* sol;
    Array* unused = arr_pop(Array*, st);
    Array* used = arr_pop(Array*, st);
    if (!is_lastly_added_ok(points, obstacles, used)){
        return new_arr();
    }
    if (unused->len == 0){
        sol = arr_cpy(int, used);
        if (is_sol_valid(points, obstacles, sol)){
            return sol;
        }else{
            free_arr(sol);
            return new_arr();
        }
    }else{
        bool* banned = calloc(n, sizeof(bool));
        Array* n_used;
        Array* n_unused;
        Array* n_st;
        int p1, p2, line;
        int checked = 0;
        p1 = arr_last(int, used);
        while(checked<unused->len){
            int score = 100000;
            int worst_p = -1;
            for(int i = 0; i<unused->len; i++){
                if (banned[i]) continue;
                //Get max from score...
                p2 = arr_at(int, unused, i);
                if (p1 == p2) continue;
                line = max(p1, p2)+n*min(p1, p2);
                if (arr_at(int, ls, line) < score){
                    score = arr_at(int, ls, line);
                    worst_p = i;
                }
            }
            //Use the biggest score/worst point, I guess
            banned[worst_p] = true;
            n_st = new_arr();
            n_used = arr_cpy(int, used);
            n_unused = arr_cpy(int, unused);
            int p = arr_at(int, n_unused, worst_p);
            arr_push(int, n_used, p);
            arr_del(int, n_unused, worst_p);
            arr_push(Array*, n_st, n_used);
            arr_push(Array*, n_st, n_unused);
            sol = best_lines_rec(points, obstacles, n_st, ls);
            free_arr(n_st);
            checked++;
            if (sol->len > 0){
                return sol;
            }else{
                free_arr(sol);
            }
        }
        free(banned);
        return new_arr();
    }
}

Array* best_lines_first(Array* points, Array* obstacles){
    int n = points->len;
    int o = obstacles->len;
    Array* ans = new_arr();
    Array* sol;
    Array* st;
    Array* used;
    Array* unused;
    Array* pts = new_arr();
    for (int i = 0; i<n; i++){
        arr_push(int, pts, i);
    }
    Array* done = new_arr_len(bool, n);
    for (int i = 0; i<n; i++) arr_push(Array*, ans, new_arr());
    if (!points_outside_obstacles(points, obstacles)){
        printf("A point is inside an obstacle, answer doesn't exist!\n");
        return ans; 
    }
    //Line score, a score for each line. Bigger score, more possible trouble! O(n^2(o+n^2))
    Array* ls = get_line_score(points, obstacles);
    for (int i = 0; i<n; i++){
        if (arr_at(bool, done, i)) continue;
        st = new_arr();
        used = new_arr();
        arr_push(int, used, i);
        unused = arr_cpy(int, pts);
        arr_del(int, unused, i);
        arr_push(Array*, st, used);
        arr_push(Array*, st, unused);
        sol = best_lines_rec(points, obstacles, st, ls);
         if (sol->len > 0){
            int lst = arr_last(int, sol);
            if(!arr_at(bool, done, i)){
                arr_set(Array*, ans, i, sol);
                arr_set(bool, done, i, true);
            }
            if(!arr_at(bool, done, lst)){
                arr_set(Array*, ans, lst, sol);
                arr_set(bool, done, lst, true);
            }
        }else{
            printf("WORST_LINE_PRIO failed to find any answer for point #%d\n", i+1);
        }
    }
    return ans;
}

Array* solve(char method[100], Array* points, Array* obstacles){
    Array* ans;
    if (strcmp(method, "brute-force") == 0){
        ans = brute_force(points, obstacles);
    }else if (strcmp(method, "brute-dfs") == 0) {
        ans = brute_dfs(points, obstacles);
    }else{
        ans = best_lines_first(points, obstacles);
    }
    return ans;
}

void draw_sol(Array* sol, Array* points){
    for (int i = 0; (i+1)<sol->len; i++){
        Vector2 p1 = arr_at(Vector2, points, arr_at(int, sol, i));
        Vector2 p2 = arr_at(Vector2, points, arr_at(int, sol, i+1));
        DrawLineEx(p1, p2, 3.0, GREEN);
    }
}

void draw_points(Array* points, int picked){
    char str[3];
    for_arr(Vector2, points, i, point,
        if (i == picked)
            DrawCircleV(point, 10.0, PURPLE);
        else
            DrawCircleV(point, 10.0, RED);
        sprintf(str, "%lld", i+1);
        DrawText(str, point.x-6, point.y-8, 24, BLACK);
    )
}

void draw_obstacles(Array* obstacles){
    Array* triangle;
    Vector2 v1, v2, v3, mid;
    char str[3];
    for (int i = 0; i<obstacles->len; i++){
        triangle = arr_at(Array*, obstacles, i);
        v1 = arr_at(Vector2, triangle, 0);
        v2 = arr_at(Vector2, triangle, 1);
        v3 = arr_at(Vector2, triangle, 2);
        mid = arr_at(Vector2, triangle, 3);
        DrawTriangle(v3, v2, v1, SKYBLUE);
        DrawTriangleLines(v1, v2, v3, BLACK);
        sprintf(str, "%lld", i+1);
        DrawText(str, mid.x-5, mid.y-5, 24, BLUE);
    }
}

int main(){
    //Set seed for random number generator
    SetRandomSeed(time(0));
    //Solve problem
    Array* points = new_arr();
    int n = 15;
    int satisfied = 0;
    for (int i = 0; i<n; i++){
        arr_push(Vector2, points, ((Vector2){(float)GetRandomValue(50,1050), (float)GetRandomValue(50, 1050)}));
    }
    Array* obstacles = new_arr();
    int o = 6;
    for (int i = 0; i<o; i++){
        Vector2 mid = {(float)GetRandomValue(250,850), (float)GetRandomValue(250, 850)};
        Array* tri = new_arr();
        arr_push(Vector2, tri, ((Vector2){(float)GetRandomValue(-100,-50)+mid.x, (float)GetRandomValue(0, 100)+mid.y}));
        arr_push(Vector2, tri, ((Vector2){(float)GetRandomValue(-49,49)+mid.x, (float)GetRandomValue(-100, 0)+mid.y}));
        arr_push(Vector2, tri, ((Vector2){(float)GetRandomValue(50,100)+mid.x, (float)GetRandomValue(0, 100)+mid.y}));
        arr_push(Vector2, tri, mid);
        arr_push(Array*, obstacles, tri);
    }
    bool recalc = false;
    bool new_points = false;
    bool new_obstacles = false;
    char method[100] = "best-lines";
    //strcpy(method, "brute-dfs");
    //Create window
    const int scrW = 1100;
    const int scrH = 1100;
    int disp_ans = 0;
    InitWindow(scrW, scrH, "Infinite Snake Problem");
    SetTargetFPS(120);
    Array* ans = solve(method, points, obstacles);
    disp_ans = n+1;
    satisfied = 0;
    for (int i = 0; i<n; i++){
        Array* sol = arr_at(Array*, ans, i);
        if (sol->len > 0){
            if (disp_ans == n+1) disp_ans = i;
            satisfied++;
        } 
    }
    //Loop
    double time_spent;
    char txt[100];
    while (!WindowShouldClose()){
        //float dt = GetFrameTime();
        //Update
        if (IsKeyPressed(KEY_RIGHT)){
            disp_ans = min(disp_ans+1, ans->len-1);
        }else if (IsKeyPressed(KEY_LEFT)){
            disp_ans = max(disp_ans-1, 0);
        }else if (IsKeyPressed(KEY_M)){
            n++;
            recalc = true;
            new_points = true;
        }else if (IsKeyPressed(KEY_B)){
            n--;
            recalc = true;
            new_points = true;
        }else if (IsKeyPressed(KEY_P)){
            o++;
            recalc = true;
            new_obstacles = true;
        }else if (IsKeyPressed(KEY_I)){
            o--;
            recalc = true;
            new_obstacles = true;
        }else if (IsKeyPressed(KEY_N)){
            recalc = true;
            new_points = true;
        }else if (IsKeyPressed(KEY_O)){
            new_obstacles = true;
            recalc = true;
        }else if (IsKeyPressed(KEY_R)){
            new_obstacles = true;
            new_points = true;
            recalc = true;
        }else if (IsKeyPressed(KEY_F1)){
            recalc = true;
            strcpy(method, "brute-force");
        }
        else if (IsKeyPressed(KEY_F2)){
            recalc = true;
            strcpy(method, "brute-dfs");
        }
        else if (IsKeyPressed(KEY_F3)){
            recalc = true;
            strcpy(method, "best-lines");
        }
        if (recalc){
            free_arr(ans);
            ans = new_arr();
            //Gen random points
            if (new_points){
                free_arr(points);
                points = new_arr();
                for (int i = 0; i<n; i++){
                    arr_push(Vector2, points, ((Vector2){(float)GetRandomValue(50,1050), (float)GetRandomValue(50, 1050)}));
                }
            }
            //Gen random obstacles
            if (new_obstacles){
                free_arr(obstacles);
                obstacles = new_arr();
                for (int i = 0; i<o; i++){
                    Vector2 mid = {(float)GetRandomValue(50,1050), (float)GetRandomValue(50, 1050)};
                    Array* tri = new_arr();
                    arr_push(Vector2, tri, ((Vector2){(float)GetRandomValue(-100,-50)+mid.x, (float)GetRandomValue(0, 100)+mid.y}));
                    arr_push(Vector2, tri, ((Vector2){(float)GetRandomValue(-49,49)+mid.x, (float)GetRandomValue(-100, 0)+mid.y}));
                    arr_push(Vector2, tri, ((Vector2){(float)GetRandomValue(50,100)+mid.x, (float)GetRandomValue(0, 100)+mid.y}));
                    arr_push(Vector2, tri, mid);
                    arr_push(Array*, obstacles, tri);
                }
            }
            disp_ans = n+1;
            satisfied = 0;
            clock_t begin = clock();
            ans = solve(method, points, obstacles);
            clock_t end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("Time: %fs.\n", time_spent);
            for (int i = 0; i<n; i++){
                Array* sol = arr_at(Array*, ans, i);
                if (sol->len > 0){
                    if (disp_ans == n+1) disp_ans = i;
                    satisfied++;
                } 
            }
            new_points = false;
            new_obstacles = false;
            recalc = false;
        }
        //Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            if (disp_ans<n){
                draw_sol(arr_at(Array*, ans, disp_ans), points);
                sprintf(txt, "Showing solution for point #%d\nn=%d, o = %d\nMethod: %s\n%d points satisfied!\nTime:%fs", disp_ans+1, (int)ans->len, o, method, satisfied, time_spent);
            }else{
                sprintf(txt, "No solutions found!\nn=%d, o = %d\nMethod: %s\nTime:%fs", (int)ans->len, o, method, time_spent);
            }
            draw_obstacles(obstacles);
            draw_points(points, disp_ans);
            DrawText(txt, 10, 10, 24, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}