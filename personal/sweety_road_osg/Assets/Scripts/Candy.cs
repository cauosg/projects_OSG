using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Candy : MonoBehaviour
{

    public GameObject[] Candies;


    public float accel_factor = 0.00f;//1000으로 나눠줄 예정
    public float max_spd = 0.2f;
    public float zorder = -0.2f;
    public float Stop_anim_length = 1.0f;
    public float Stop_anim_spd = 1.0f;
    public float Stop_anim_power = 0.2f;//10으로 나눔
    public float stay_range = 0.2f;
    public float change_spd = 1.0f;
    public float change_sensitivity = 4;
    public float lerp_end_thres = 0.1f;
    public bool ready_to_bomb = false;// { get { return ready_to_bomb; } set { ready_to_bomb = value; } }

    private GameObject its_Candy;
    public int this_type;
    private bool is_lerp, update_neighbors, is_show, is_stop_anim, is_moved, allow_change, is_change, check_once, do_cancel, is_change_name = false;
    private float spd = 0.000f;
    private float Stop_anim_time = 0.00f;
    private float scale_factor = 0;
    private float interval;
    private bool is_update;
    private Candy_dispenser dispenser;

    public Candy[] neighbors;

    public enum dir_4 { N, S, W, E };
    public enum check_dir { row, col };
    //private string[] neighbors;
    private string name_to_change;
    private dir_4 now_dir;
    //private bool[] movablity;
    private int ind_y, ind_x, width, height, next_x, next_y;
    private Vector2 begin_touch, end_touch, now_touch;
    private Vector3 dest_pos, now_pos, change_dir, back_pos;
    private int row_score, col_score;
    //private string[] other_neighbors;
    private List<Candy> my_powder;
    //private List<string> bullet_powder;
    /*
     * type :
     basic:
     * B(blue) - 0
     * R(red) - 1
     * G(green) - 2
     * Y(yellow) - 3
     * P(Purple) - 4
     * S(sausage) - 5
     special:
     * C(ball candy)
     * Rainbow
     additional:
     * ribbon x,y
         
     */
    // Use this for initialization
    void Start()
    {
        my_powder = new List<Candy>();
        neighbors = new Candy[4];
        do_cancel = false;
        //movablity = new bool[4];
        dispenser.powders = new List<Candy>();
        accel_factor *= 0.001f;
        Stop_anim_power *= 0.1f;
    }

    private void OnMouseDown()
    {
        Debug.Log(this.gameObject.name);
        //Debug.Log("x:" + ind_x + ", y:" + ind_y);

        begin_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
    }
    private void OnMouseUp()
    {
        end_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
       
        Calc_angle();

    }

    private void OnMouseDrag()
    {
        now_touch = Camera.main.ScreenToWorldPoint(Input.mousePosition);
    }
    private float Calc_length(Vector2 a, Vector2 b)
    {
        return (b - a).magnitude;
    }

    private void Calc_angle()
    {//tan 특성상에 맞추어 좌우 구분 필요,4방향
        //
        Vector2 direction = end_touch - begin_touch;
        if (direction.magnitude < interval / change_sensitivity)
        {
            //Debug.Log("too short to change");
            return;

        }
        float dir_angle = Mathf.Atan(direction.y / direction.x);
        if (end_touch.x - begin_touch.x > 0)//우측
        {
            if (dir_angle < -1)
                now_dir = dir_4.S;//Debug.Log("S");
            else if (dir_angle < 1)
                now_dir = dir_4.E;//Debug.Log("E");
            else
                now_dir = dir_4.N;//Debug.Log("N");
        }
        else
        {
            if (dir_angle < -1)
                now_dir = dir_4.N;//Debug.Log("N");
            else if (dir_angle < 1)
                now_dir = dir_4.W;//Debug.Log("W");
            else
                now_dir = dir_4.S;//Debug.Log("S");
        }

        Request_change(now_dir);
    }
    //change next_x, next_y
    private void Translate_index(dir_4 in_dir)
    {
        switch (in_dir)
        {
            case dir_4.N:
                next_x = ind_x;
                next_y = ind_y - 1;
                break;
            case dir_4.S:
                next_x = ind_x;
                next_y = ind_y + 1;
                break;
            case dir_4.W:
                next_x = ind_x - 1;
                next_y = ind_y;
                break;
            case dir_4.E:
                next_x = ind_x + 1;
                next_y = ind_y;
                break;
            default: break;
        }

    }

    private dir_4 Opposite_dir(dir_4 in_dir)
    {
        dir_4 out_dir = 0;
        //enum Oppo{S,N,E, W}
        switch (in_dir)
        {
            case dir_4.N:
                out_dir = dir_4.S;
                break;
            case dir_4.S:
                out_dir = dir_4.N;
                break;
            case dir_4.W:
                out_dir = dir_4.E;
                break;
            case dir_4.E:
                out_dir = dir_4.W;
                break;
            default: break;
        }
        return out_dir;

    }

    private bool Is_on_proc()
    {
        bool out_bool = false;
        if (ready_to_bomb)
            out_bool = true;
        if (do_cancel)
            out_bool = true;
        return out_bool;
    }

    private void Transforming(Candy other_candy)//backup and change
    {
         name_to_change = other_candy.name;
        //name_to_change = this.gameObject.name;
        //this.gameObject.name = other_candy.name;

        dest_pos = other_candy.transform.position;//other_candy.transform.position;
        //Debug.Log(this.transform.position);
        now_pos = this.transform.position;

        //int temp2 = ind_x;
        //ind_x = other_candy.ind_x;
        //next_x = temp2;
        next_x = other_candy.ind_x;
        //temp2 = ind_y;
        //ind_y = other_candy.ind_y;
        //next_y = temp2;
        next_y = other_candy.ind_y;
    }
    private void Cancel_change()
    {
        do_cancel = false;
        //Debug.Log(gameObject.name + " s cancel is started, name to be changed is " + name_to_change);
        //this.transform.name = name_to_change;
        name_to_change = null;
        //Debug.Log(now_pos);
        Vector3 temp = now_pos;
        now_pos = dest_pos;
        dest_pos = temp;

        //Debug.Log(now_pos);
        //Debug.Log(dest_pos);
        //ind_x = next_x;
        //ind_y = next_y;

        next_x = ind_x;
        next_y = ind_y;

        my_powder.Clear();

        Begin_Lerp_change();
    }

    private void Refresh_after_change()
    {
        //Debug.Log("cancel or not of " + this.gameObject.name + " is " + do_cancel); 
        //if (!Is_on_proc())
        //    return;
        //is_change = false;//?

        if (name_to_change != null)
        {
            this.gameObject.name = name_to_change;
            name_to_change = null;
        }
        back_pos = this.transform.position;

        ind_x = next_x;
        ind_y = next_y;

        //if (ready_to_bomb)
        //    Fire();
        //if (!ready_to_bomb)
        //    my_powder.Clear();
        my_powder.Clear();
        Refresh_neighbors();
        //Debug.Log(gameObject.name + " s ind : [" + ind_x + "][" + ind_y + "]");

        Check_score();
        //is_cancel = false;

        Change_is_drop(false);
    }

    private void Begin_Lerp_change()
    {
        //Debug.Log("Lerp move is begin at " + this.gameObject.name);
        change_dir = dest_pos - now_pos;
        //Debug.Log(gameObject.name + "'s others position is" + dest_pos);
        //Debug.Log(gameObject.name + "'s now position is" + now_pos);
        //Debug.Log(gameObject.name + "'s position is" + this.transform.position);
        is_change = true;
    }

    private void Lerp_change()
    {
        this.transform.Translate(change_dir * Time.deltaTime * change_spd);
        //Debug.Log(this.gameObject.name + " is changing");
        //Change_is_drop(true);
        if ((this.transform.position - dest_pos).magnitude < lerp_end_thres)
        {
            //Debug.Log(gameObject.name + "'s lerp is end");
            is_change = false;
            this.transform.position = dest_pos;

            if (do_cancel)
                Cancel_change();
            else
                Refresh_after_change(); 
        }
    }

    private void Request_change(dir_4 direction)
    {
        //
        //GameObject temp = GameObject.Find(neighbors[(int)now_dir]);
        int now_ind = (int)now_dir;
        Refresh_neighbors();
        //for(int i = 0; i< 4; i++)
        //{
        //    Debug.Log(neighbors[i]);
        //}

        if (neighbors[now_ind] == null)// || !movablity[(int)direction])
        {
            Debug.Log(direction + ", theres no neighbot");
            return;
        }
        Candy to_memorize = neighbors[now_ind];
        to_memorize.Refresh_neighbors();
        //Debug.Log(to_memorize.name);

        Candy[] other_neighbors = neighbors[now_ind].neighbors;
        other_neighbors[(int)Opposite_dir(direction)] = neighbors[now_ind];//바꼈을때 상황 적용

        bool temp = neighbors[now_ind].Response_change(this, direction);
        if (Check_score(other_neighbors,to_memorize) || temp)
            do_cancel = false;
        else
            do_cancel = true;


        //Debug.Log(do_cancel);
        //Debug.Log(to_memorize.name);
        to_memorize.do_cancel = do_cancel;

        Transforming(to_memorize);

        Begin_Lerp_change();
        //Debug.Log(this.gameObject.name + " will be changed");
    }

    public bool Response_change(Candy other_candy, dir_4 direction)//바꿔도 되는지 응답,이전 멤버 명칭 포함
    {
        //Refresh_neighbors();
        Candy to_memorize = other_candy;

        Candy[] other_neighbors = other_candy.neighbors;
        //Debug.Log(other_candy.transform.position);
        other_neighbors[(int)direction] = other_candy;//바꼈을때 상황 적용

        bool out_bool = Check_score(other_neighbors, other_candy);

        Transforming(to_memorize);
        Begin_Lerp_change();

        return out_bool;
    }

    public void Response_cancel(bool bool_cancel)//바꿔도 되는지 응답,이전 멤버 명칭 포함
    {
        if (!do_cancel)
            return;
        this.do_cancel = bool_cancel;

        Debug.Log(gameObject.name + " is got respond");

        Cancel_change();
    }

  


    public void Move_to_new(int i, int j, Vector3 term_pos, int steps)
    {
        Begin_drop(this.transform.position, term_pos, steps);

        gameObject.name = Name_translate(i, j);
        ind_x = i;
        ind_y = j;
        Refresh_neighbors();
        //name_to_change = Name_translate(i, j);
        //next_x = i;
        //next_y = j;

    }


    private void Stop_anim()
    {
        Change_is_drop(true);
        if (Stop_anim_time > Stop_anim_length)
        {
            Stop_anim_time = 0.00f;
            is_stop_anim = false;
            Change_is_drop(false);
            //Refresh_neighbors();
            Check_score();//Refresh_after_change();
        }
        Stop_anim_time += Time.deltaTime;
        its_Candy.transform.localScale = new Vector2(1 + Stop_anim_power * Mathf.Sin(Mathf.PI / 2 * Stop_anim_spd * Stop_anim_time),
            1 - Stop_anim_power * Mathf.Sin(Mathf.PI / 2 * Stop_anim_spd * Stop_anim_time)) * scale_factor;
        //Debug.Log(this.transform.position);
        its_Candy.transform.position = this.transform.position - new Vector3(0, scale_factor - its_Candy.transform.localScale.y, 0);

    }
    private void End_lerp()
    {
       // Debug.Log(gameObject.name + "' s lerp_end");
        spd = 0.000f;
        is_lerp = false;
        this.back_pos = dest_pos;
        this.transform.position = dest_pos;
        is_stop_anim = true;

        //Refresh_neighbors();
        //Refresh_after_change();
        //dispenser.is_update = true;
    }

    private void Change_is_drop(bool in_bool)
    {
        if (dispenser.is_drop != in_bool)
            dispenser.is_drop = in_bool;
        is_update = false;
    }
    private void Lerp_move()
    {
        //Change_is_drop(true);
        //Debug.Log("im moving now....");
        spd += accel_factor * Time.deltaTime;
        if (spd > max_spd)
            spd = max_spd;

        this.transform.Translate(new Vector2(0, -spd));
        //Debug.Log(this.transform.position);
        if ((this.transform.position - dest_pos).magnitude < lerp_end_thres)
            End_lerp();
        if (!is_show && this.transform.position.y < back_pos.y)
            its_Candy.GetComponent<SpriteRenderer>().enabled = true;
    }

    private void Begin_drop(Vector2 begin_pos, Vector2 target_pos, int qs)
    {
        this.back_pos = new Vector3(begin_pos.x, begin_pos.y, zorder);
        //this.transform.position = begin_pos
        this.dest_pos = new Vector3(target_pos.x, target_pos.y, zorder);
        this.transform.position = dest_pos + new Vector3(0, qs * interval, 0);
        this.is_lerp = true;
        //Debug.Log("drop");
    }

    private string Name_translate(int i, int j)
    {
        return "Candy[" + i + "][" + j + "]";
    }

    public void Init(int candy_type, Vector2 begin_pos, Vector2 target_pos, float interv, int i, int j, int queues)
    {
        //Debug.Log(candy_type);
        //Debug.Log(Candies[candy_type]);

        dispenser = GameObject.Find("Candy_dispenser").GetComponent<Candy_dispenser>();
        gameObject.name = Name_translate(i, j);
        //allow_change = true;//for debug
        height = GameObject.Find("Level_manager").GetComponent<Level>().BackTile_height;
        width = GameObject.Find("Level_manager").GetComponent<Level>().BackTile_width;
        this.ind_y = j;
        this.ind_x = i;
        this.interval = interv;
        this.this_type = candy_type - 2;//0과 1인거 제외하기위해 보정
        stay_range *= interval;


        its_Candy = Instantiate(Candies[this_type], new Vector3(0,0,0), Quaternion.identity) as GameObject;
        back_pos = this.transform.position;

        scale_factor = interval / its_Candy.GetComponent<SpriteRenderer>().bounds.size.x;
        its_Candy.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
        its_Candy.transform.SetParent(this.transform);
        its_Candy.transform.Translate(0, 0, zorder);
        its_Candy.GetComponent<SpriteRenderer>().enabled = false;

        Begin_drop(begin_pos, target_pos, queues);
    }


    private void bullet_load(Candy item)
    {
        if (my_powder.Contains(item))
            return;
        my_powder.Add(item);
    }


    private void bullet_send()
    {
        //if (!dispenser.powders.Contains(item))
        //    dispenser.powders.Add(item);
        dispenser.powders.AddRange(my_powder);
    }

    private void bullet_trigger()
    {
        if (!ready_to_bomb)
            ready_to_bomb = true;
    }

    private void Fire()
    {
        //Debug.Log(this.gameObject.name + " got fired");
        bullet_load(this);
        bullet_send();
        dispenser.Explode_candies();
        my_powder.Clear();
        ready_to_bomb = false;
    }

    private bool Check_Candy_ball()
    {
        Refresh_neighbors();
        bool[] same_hood = new bool[4];
        //for (int i = 0; i < 4; i++)
        //    same_hood[i] = false;
        //Candy[] same_candies = new Candy[4];

        for (int i = 0; i < 4; i++) {
            same_hood[i] = false;
            //GameObject neighbor_candy = GameObject.Find(neighbors[i]);
            if (neighbors[i] == null)
                continue;

            if (!neighbors[i].GetComponent<Candy>().Are_u_same(this_type))
                continue;
            same_hood[i] = true;
            //Debug.Log(gameObject.name + "'s neighbor of " + (dir_4)i + " is same");
            //same_candies[i] = neighbors[i].GetComponent<Candy>();
        }
        bool checker = false;

        //2x2중 ㄹ자 포함시키지 않으므로 제외
        //정확히 2x2만을 탐색
        
        if (same_hood[0] && same_hood[2])
        {
            GameObject temp = GameObject.Find(Name_translate(ind_x - 1, ind_y - 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (same_hood[0] && same_hood[3])
        {
            GameObject temp = GameObject.Find(Name_translate(ind_x + 1, ind_y - 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (same_hood[1] && same_hood[2])
        {
            GameObject temp = GameObject.Find(Name_translate(ind_x - 1, ind_y + 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (same_hood[1] && same_hood[3])
        {
            GameObject temp = GameObject.Find(Name_translate(ind_x + 1, ind_y + 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (checker)
        {
            Check_connected();
            //Debug.Log(gameObject.name + " s ind : [" + ind_x + "][" + ind_y + "]");
        }
        //Debug.Log(checker);
        return checker;
    }

    private bool Check_Candy_ball(Candy[] others_hood,Candy other)
    {
        //Refresh_neighbors();
        bool[] same_hood = new bool[4];
        //for (int i = 0; i < 4; i++)
        //    same_hood[i] = false;
        //Candy[] same_candies = new Candy[4];

        for (int i = 0; i < 4; i++)
        {
            same_hood[i] = false;
            //GameObject neighbor_candy = GameObject.Find(neighbors[i]);
            if (others_hood[i] == null)
                continue;

            if (!others_hood[i].GetComponent<Candy>().Are_u_same(this_type))
                continue;
            same_hood[i] = true;
            //Debug.Log(gameObject.name + "'s neighbor of " + (dir_4)i + " is same");
            //same_candies[i] = neighbors[i].GetComponent<Candy>();
        }
        bool checker = false;

        //2x2중 ㄹ자 포함시키지 않으므로 제외
        //정확히 2x2만을 탐색

        if (same_hood[0] && same_hood[2])
        {
            GameObject temp = GameObject.Find(Name_translate(other.ind_x - 1, other.ind_y - 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (same_hood[0] && same_hood[3])
        {
            GameObject temp = GameObject.Find(Name_translate(other.ind_x + 1, other.ind_y - 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (same_hood[1] && same_hood[2])
        {
            GameObject temp = GameObject.Find(Name_translate(other.ind_x - 1, other.ind_y + 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        if (same_hood[1] && same_hood[3])
        {
            GameObject temp = GameObject.Find(Name_translate(other.ind_x + 1, other.ind_y + 1));
            if (temp != null)
                checker = temp.GetComponent<Candy>().Are_u_same(this_type);
        }
        //if (checker)
        //{
        //    Check_connected();
        //    Debug.Log(gameObject.name + " s ind : [" + other.ind_x + "][" + other.ind_y + "]");
        //}
        //Debug.Log(checker);
        return checker;
    }

    private void Check_connected()
    {
        Check_all_dir(ref my_powder);

        //Debug.Log("candy_ball is made by " + this.gameObject.name);
        //for (int i = 0; i < my_powder.Count; i++)
        //    Debug.Log("Candy ball elem of " + this.gameObject.name + " is " + my_powder[i].name + " and type is " + my_powder[i].this_type);
        Fire();
    }

    public bool Check_all_dir(ref List<Candy> parent_powder)//,override, Candyball check만을 위함
    {
        //Debug.Log("myname is " + gameObject.name + " and other : " + except_name);
        Refresh_neighbors();
        int same_count = 0;//stackoverflow 방지

        for (int i = 0; i < 4; i++)
        {
            if (neighbors[i] == null)
                continue;
            if (parent_powder.Contains(neighbors[i]))//stackoverflow 방지
                continue;

            //Candy neighbor_candy = temp.GetComponent<Candy>();
            if (neighbors[i].Are_u_same(this_type))
            {
                parent_powder.Add(neighbors[i]);
                neighbors[i].Check_all_dir(ref parent_powder);
            }
        }

        return false;
    }

    public bool Check_all_dir(ref int row_score, ref int col_score)//,override
    {
        Refresh_neighbors();
        for (int i = 0; i < 4; i++)
        {
            //Debug.Log(neighbors[i]);
            //GameObject neighbor_candy = GameObject.Find(neighbors[i]);
            if (neighbors[i] == null)
                continue;
             if (my_powder.Contains(neighbors[i]))//feedback방지
                continue;
            //if (neighbors[i].gameObject.name == this.gameObject.name)//feedback방지
            //    continue;
            //Debug.Log(this.gameObject.name + "candy will check " + neighbors[i]);

            if (!neighbors[i].Are_u_same(this_type))
                continue;

            //Debug.Log(this.gameObject.name + "is same with  " + neighbors[i].gameObject.name + "by " + (dir_4)i);

            //if (!my_powder.Contains(neighbors[i]))
                my_powder.Add(neighbors[i]);
            //dispenser.powders.Add(neighbors[i]);


            if (i > 1)
                row_score++;
            else
                col_score++;
            neighbors[i].Check_all_dir(this_type, (dir_4)i, ref row_score, ref col_score, ref my_powder);
        }
        
        return false;
    }

    public bool Check_all_dir(Candy[] other_neighbors, ref int row_score, ref int col_score)//,override
    {
        //for (int i = 0; i < 4; i++)
        //    if (other_neighbors[i] != null)
        //        Debug.Log(this.gameObject.name + " 's neibor of dir " + (dir_4)i + " is " + other_neighbors[i].name);


        for (int i = 0; i < 4; i++)
        {
            //GameObject neighbor_candy = GameObject.Find(other_candy.other_neighbors[i]);

            if (other_neighbors[i] == null)
                continue;

            if (!other_neighbors[i].Are_u_same(this_type))
                continue;

            //Debug.Log(this.gameObject.name + " 'is same with by" + (dir_4)i + " to " + other_neighbors[i].name);
            my_powder.Add(other_neighbors[i]);

            if (i > 1)
                row_score++;
            else
                col_score++;
            other_neighbors[i].Check_all_dir(this_type, (dir_4)i, ref row_score, ref col_score, ref my_powder);
        }

        return false;
    }

    public bool Check_all_dir(int check_type, dir_4 prev_dir, ref int row_score, ref int col_score, ref List<Candy> parent_powder)//, ref int org_type)
    {
        if (parent_powder.Contains(neighbors[(int)prev_dir]))
            return false;

        int now_ind = (int)prev_dir;
        //GameObject neighbor_candy = GameObject.Find(neighbors[(int)prev_dir]);

        if (neighbors[now_ind] == null)
            return false;

        //Debug.Log(this.gameObject.name + "candy will check " + neighbors[i]);

        if (!neighbors[now_ind].Are_u_same(check_type))
            return false;

        if (!parent_powder.Contains(neighbors[(int)prev_dir]))
            parent_powder.Add(neighbors[(int)prev_dir]);

        if (now_ind > 1)
            row_score++;
        else
            col_score++;
        if(!neighbors[now_ind].Check_all_dir(this_type, prev_dir, ref row_score, ref col_score, ref my_powder))
            return false;
        return true;
    }

    public bool Are_u_same(int candy_type)
    {
        //Debug.Log("my:" + this_type + ", you: " + candy_type);

        if (candy_type != this_type)
            return false;

        return true;

    }



    private void Check_score()
    {
        my_powder.Clear();

        Refresh_neighbors();
        row_score = 1;
        col_score = 1;

        if (!Check_Candy_ball())
            Check_all_dir(ref row_score, ref col_score);
        else
            return;

        //if (Check_Candy_ball())
        //    row_score = 999;
        //else
        //    Check_all_dir(ref row_score, ref col_score);

        //Debug.Log(gameObject.name + "'s row_score:" + row_score + ", col_score: " + col_score);// +  "i+1 : " + (dir_4)(1 + 2));

        if (row_score > 2 || col_score > 2)
        {
            Fire();
        }
    }

    private bool Check_score(Candy[] other_neighbors, Candy other)//swipe 검사하기 위해 override
    {
        my_powder.Clear();
        //Refresh_neighbors();

        row_score = 1;
        col_score = 1;
        if (!Check_Candy_ball(other_neighbors,  other))
            Check_all_dir(other_neighbors, ref row_score, ref col_score);
        else
            return true;
       

        Debug.Log(gameObject.name + "'s row_score:" + row_score + ", col_score: " + col_score);// +  "i+1 : " + (dir_4)(1 + 2));

        if (row_score > 2 || col_score > 2)
        {
            //bullet_trigger();
            return true;
        }
        return false;

    }

    public void Bomb()
    {
        dispenser.Refill_plz(this);
        //Debug.Log("candy " + gameObject.name + "requested refill");
        Destroy(this.gameObject);
    }


    public void Refresh_neighbors()
    {
        //movablity = new bool[4];

        //if (ready_to_bomb)
        //{
        //    Fire();
        //    ready_to_bomb = false;
        //}


        neighbors = new Candy[4];

        string now_name = Name_translate(ind_x, ind_y - 1);
        GameObject temp = GameObject.Find(now_name);
        if (temp != null)//N
            neighbors[0] = temp.GetComponent<Candy>();
        else
            neighbors[0] = null;

        //S
        now_name = Name_translate(ind_x, ind_y + 1);
        temp = GameObject.Find(now_name);
        if (temp != null)
            neighbors[1] = temp.GetComponent<Candy>();
        else
            neighbors[1] = null;
        //W
        now_name = Name_translate(ind_x - 1, ind_y);
        temp = GameObject.Find(now_name);
        if (temp != null)//W
            neighbors[2] = temp.GetComponent<Candy>();
        else
            neighbors[2] = null;
        //E
        now_name = Name_translate(ind_x + 1, ind_y);
        temp = GameObject.Find(now_name);
        if (temp != null)//E
            neighbors[3] = temp.GetComponent<Candy>();
        else
            neighbors[3] = null;

        //for (int i = 0; i < 4; i++)
        //    if (neighbors[i] != null)
        //        Debug.Log(this.gameObject.name + " 's neibor of dir " + (dir_4)i + " is " + neighbors[i].name);

        //bullet_powder.Clear();
    }


    public int Get_ind_x()
    {
        return ind_x;
    }

    public int Get_ind_y()
    {
        return ind_y;
    }

    public string Get_name()
    {
        return this.gameObject.name;
    }

    // Update is called once per frame
    void Update()
    {
        if (is_lerp)
        {
            Lerp_move();
            return;
        }
        if (is_stop_anim)
            Stop_anim();
        if (is_change)
            Lerp_change();
        if (!dispenser.is_drop)
        {
            Refresh_neighbors();
            //Check_score();
        }
        //if (ready_to_bomb)
        //    ready_to_bomb = false;
        //if (dispenser.is_update)
        //{
        //    //Debug.Log("now_checking");
        //    Refresh_neighbors();
        //}
    }
}
