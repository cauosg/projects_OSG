using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Candy : MonoBehaviour
{

    public GameObject[] Candies;
    public GameObject[] Specials;
    public GameObject[] Additional;

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
    public int candy_ball_score = 4;
    public float roll_spd = 5.0f;

    public bool ready_to_bomb = false;// { get { return ready_to_bomb; } set { ready_to_bomb = value; } }

    private GameObject its_Candy,its_Special, its_addy;
    public types this_type;
    public additional addy;
    private bool is_lerp, update_neighbors, is_show, is_stop_anim, is_moved, allow_change, is_change, check_once, do_cancel, is_change_name, is_roll, is_wait = false;
    private float spd = 0.000f;
    private float Stop_anim_time = 0.00f;
    private float scale_factor = 0;
    private float interval;
    private float wait_time = 0;
    private Candy_dispenser dispenser;

    public Candy[] neighbors;

    public enum dir_4 { N, S, W, E }
    public enum Special_list { Ball, Rainbow };
    public enum check_dir { row, col };
    public enum types {Blue, Red, Green,Yellow, Purple, Sausage, Ball, Rainbow};
    public enum additional { none, Rx, Ry, Sq};
    //private string[] neighbors;
    private string name_to_change;
    //private dir_4 now_dir;
    //private bool[] movablity;
    private int ind_y, ind_x, width, height, next_x, next_y, special_score;
    private Vector2 begin_touch, end_touch, now_touch, roll_dir, dest_hole;
    private Vector3 dest_pos, now_pos, change_dir, back_pos, roll_axis;
    //private int row_score, col_score;
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
     * candyball(먼치킨) - 6
     special:
     * C(ball candy)
     * Rainbow
     additional:
     * ribbon x
     * ribbon y
     * square    
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


    private bool Check_additinal(List<Candy> rows, List<Candy> cols)//특수블록 생성, 각각 우선순위 고려할것
    {
        Debug.Log("row score is :" + rows.Count);
        Debug.Log("col score is :" + cols.Count);

        if(rows.Count > 3 || cols.Count > 3)
        {
            Rainbow_form();
            return true;
        }
        if (rows.Count > 1 && cols.Count > 1)
        {
            Specialize(additional.Sq);
            return true;
        }

        if (rows.Count > 2)
        {
            Specialize(additional.Rx);
            return true;
        }
        if (cols.Count > 2)
        {
            Specialize(additional.Ry);
            return true;
        }
        return false;
    }

    public void Specialize(additional in_type)//특수형 생성
    {
        this.addy = in_type;
        its_addy = Inst_candy(Additional[(int)in_type - 1], zorder - 0.1f);
        //switch (in_type)
        //{
        //    case additional.Rx:
        //        its_addy = Inst_candy(Additional[(int)in_type], zorder - 0.1f);
        //        break;
        //    case additional.Ry:
        //        its_addy = Inst_candy(Additional[(int)in_type], zorder - 0.1f);
        //        break;
        //    case additional.Sq:
        //        its_addy = Inst_candy(Additional[(int)in_type], zorder - 0.1f);
        //        break;
        //    default: break;
        //}
    }

    private void Rainbow_bomb(Candy other)
    {
        types bomb_type = other.this_type;
        additional other_addy = other.addy;

        for(int i = 0;i<width; i++)
            for(int j = 0; j<height; j++)
            {

            }
    }
    public void Rainbow(dir_4 direction)
    {
        int now_ind = (int)direction;
        Refresh_neighbors();

        if (neighbors[now_ind] == null)// || !movablity[(int)direction])
        {
            Debug.Log(direction + ", theres no neighbor");
            return;
        }
        Candy to_memorize = neighbors[now_ind];
        
        Transforming(to_memorize);

        Begin_Lerp_change();
        //Debug.Log(this.gameObject.name + " will be changed");
    }

    public void Ballize()//먼치킨으로 변환
    {
        Debug.Log(this.gameObject.name + " is ballized"); ;
        //its_Special = Inst_ball(zorder);
        its_Special = Instantiate(Specials[(int)Special_list.Ball], this.transform.position, Quaternion.identity) as GameObject;

        float sf = interval / its_Special.GetComponent<MeshRenderer>().bounds.size.x;
        its_Special.transform.localScale = new Vector3(sf, sf, sf);
        its_Special.transform.SetParent(this.transform);
        its_Special.transform.Translate(0, 0, zorder);

        this_type = types.Ball;
        its_Candy.GetComponent<SpriteRenderer>().enabled = false;
        //Destroy(its_Candy);
    }

    public void destroy_by_ball(int break_order)
    {
        dispenser.Refill_plz(this);
        Debug.Log(gameObject.name + " is waitting ball");
        wait_time = break_order;
        is_wait = true;
    }

    private void visible_by_time()
    {
        wait_time -= Time.deltaTime * roll_spd;
        if(wait_time<0)
        {
            is_wait = false;
            its_Candy.GetComponent<SpriteRenderer>().enabled = false;
        }
    }

    private void ball_move()
    {
        this.transform.Translate(roll_dir * Time.deltaTime * roll_spd);
        its_Special.transform.Rotate(roll_axis, Time.deltaTime * roll_spd);
        if ((this.transform.position - new Vector3(dest_hole.x, dest_hole.y, zorder)).magnitude < lerp_end_thres)
        {
            is_roll = false;
            Fire(special_score);
            Change_is_drop(false);
        }
    }

    public void ball_break(dir_4 in_dir)
    {
        Debug.Log(in_dir);
        int dest_x = 0;
        int dest_y = 0;
        int iter_x = 1;
        int iter_y = 1;
        
        switch (in_dir)
        {
            case dir_4.N:
                dest_x = 1;
                dest_y = ind_y;
                iter_x = 1;
                iter_y = -1;
                roll_dir = new Vector2(0, 1);
                roll_axis = new Vector3(-1, 0);
                dest_hole = dispenser.Get_dest_ball(ind_x,0);
                break;
            case dir_4.S:
                dest_x = 1;
                dest_y = height - ind_y;
                iter_x = 1;
                iter_y = 1;
                roll_dir = new Vector2(0, -1);
                roll_axis = new Vector3(1, 0);
                dest_hole = dispenser.Get_dest_ball(ind_x, height - 1);
                break;
            case dir_4.W:
                dest_x = ind_x;
                dest_y = 1;
                iter_x = -1;
                iter_y = 1;
                roll_dir = new Vector2(-1, 0);
                roll_axis = new Vector3(0, -1);
                dest_hole = dispenser.Get_dest_ball(0, ind_y);
                break;
            case dir_4.E:
                dest_x = width - ind_x;
                dest_y = 1;
                iter_x = 1;
                iter_y = 1;
                roll_dir = new Vector2(1, 0);
                roll_axis = new Vector3(0, 1);
                dest_hole = dispenser.Get_dest_ball(width - 1, ind_y);
                break;
            default: break;
        }
        int tot_breaks = 0;
        for(int i = 0;i<dest_x;i++)
            for(int j = 0; j<dest_y; j++)
            {
                //if (i < 0 && j < 0)
                //    continue;
                string name_to_break = Name_translate(ind_x + i*iter_x,ind_y + j*iter_y);
                Debug.Log(gameObject.name + " s name_translte : " + name_to_break);
                GameObject temp = GameObject.Find(name_to_break);
                if (temp == null)
                    continue;
                Candy temp2 = temp.GetComponent<Candy>();
                temp2.destroy_by_ball(tot_breaks++);
                Add_candy(ref my_powder, temp2);//my_powder.Add(temp2);
            }
        special_score = tot_breaks;
        is_roll = true;
    }

    public void Rainbow_form()//무지개캔디로 변환
    {
        this_type = types.Rainbow;

        Destroy(its_Candy);
        its_Candy = Inst_candy(Specials[(int)Special_list.Rainbow], zorder);
    }

    private void Translate_action(dir_4 in_dir)
    {
        switch (this_type)
        {
            case types.Ball:
                ball_break(in_dir);
                break;
            default:
                Request_change(in_dir);
                break;
        }

    }

    private void Calc_angle()
    {//tan 특성상에 맞추어 좌우 구분 필요,4방향
        //
        dir_4 now_dir;
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
        Translate_action(now_dir);
        //Request_change(now_dir);
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

    private GameObject Inst_ball(float z_priority)
    {
        GameObject out_obj = Instantiate(Specials[(int)Special_list.Ball], new Vector3(0, 0, 0), Quaternion.identity) as GameObject;

        float sf = interval / out_obj.GetComponent<MeshRenderer>().bounds.size.x;
        out_obj.transform.localScale = new Vector3(sf, sf, sf);
        out_obj.transform.SetParent(this.transform);
        out_obj.transform.Translate(0, 0, z_priority);
        

        return out_obj;
    }

    private GameObject Inst_candy(GameObject obj_type, float z_priority)
    {
        GameObject out_obj = Instantiate(obj_type, this.transform.position, Quaternion.identity) as GameObject;


        float sf = interval / out_obj.GetComponent<SpriteRenderer>().bounds.size.x;
        out_obj.transform.localScale = new Vector3(sf, sf, sf);
        out_obj.transform.SetParent(this.transform);
        out_obj.transform.Translate(0, 0, z_priority);

        return out_obj;
    }

    private GameObject Inst_candy(GameObject obj_type, float z_priority, ref float sf)
    {
        GameObject out_obj = Instantiate(obj_type, this.transform.position, Quaternion.identity) as GameObject;


        sf = interval / out_obj.GetComponent<SpriteRenderer>().bounds.size.x;
        out_obj.transform.localScale = new Vector3(sf, sf, sf);
        out_obj.transform.SetParent(this.transform);
        out_obj.transform.Translate(0, 0, z_priority);
        out_obj.GetComponent<SpriteRenderer>().enabled = false;

        return out_obj;
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
        this.this_type = (types)(candy_type - 2);//0과 1인거 제외하기위해 보정
        this.addy = additional.none;
        stay_range *= interval;


        its_Candy = Inst_candy(Candies[(int)this_type], zorder, ref scale_factor);// Instantiate(Candies[this_type], new Vector3(0,0,0), Quaternion.identity) as GameObject;
        back_pos = this.transform.position;

        //scale_factor = interval / its_Candy.GetComponent<SpriteRenderer>().bounds.size.x;


        Begin_drop(begin_pos, target_pos, queues);
    }


    private void Transforming(Candy other_candy)//backup and change
    {
        name_to_change = other_candy.name;

        dest_pos = other_candy.transform.position;//other_candy.transform.position;
        //Debug.Log(this.transform.position);
        now_pos = this.transform.position;


        next_x = other_candy.ind_x;

        next_y = other_candy.ind_y;
    }
    private void Cancel_change()
    {
        do_cancel = false;
        //Debug.Log(gameObject.name + " s cancel is started, name to be changed is " + name_to_change);
        name_to_change = null;
        //Debug.Log(now_pos);
        Vector3 temp = now_pos;
        now_pos = dest_pos;
        dest_pos = temp;

        next_x = ind_x;
        next_y = ind_y;

        my_powder.Clear();

        Begin_Lerp_change();
    }

    private void Refresh_after_change()
    {
        //Debug.Log("cancel or not of " + this.gameObject.name + " is " + do_cancel); 

        if (name_to_change != null)
        {
            this.gameObject.name = name_to_change;
            name_to_change = null;
        }
        back_pos = this.transform.position;

        ind_x = next_x;
        ind_y = next_y;

        my_powder.Clear();
        Refresh_neighbors();
        //Debug.Log(gameObject.name + " s ind : [" + ind_x + "][" + ind_y + "]");

        Check_score();

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
        int now_ind = (int)direction;
        Refresh_neighbors();

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
    }


    private void Stop_anim()
    {
        Change_is_drop(true);
        if (Stop_anim_time > Stop_anim_length)
        {
            Stop_anim_time = 0.00f;
            is_stop_anim = false;
            Change_is_drop(false);
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
    }

    private void Change_is_drop(bool in_bool)
    {
        if (dispenser.is_drop != in_bool)
            dispenser.is_drop = in_bool;
    }
    private void Lerp_move()
    {

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
        //my_powder.Clear();
        this.back_pos = new Vector3(begin_pos.x, begin_pos.y, zorder);
        this.dest_pos = new Vector3(target_pos.x, target_pos.y, zorder);
        this.transform.position = dest_pos + new Vector3(0, qs * interval, 0);
        this.is_lerp = true;
        //Debug.Log("drop");
    }

    private string Name_translate(int i, int j)
    {
        return "Candy[" + i + "][" + j + "]";
    }


    private void bullet_load(Candy item)
    {
        if (my_powder.Contains(item))
            return;
        my_powder.Add(item);
    }


    private void bullet_send(int tot_score)
    {
        dispenser.Recv_powders(my_powder, tot_score);
    }

    private void bullet_trigger()
    {
        if (!ready_to_bomb)
            ready_to_bomb = true;
    }

    private void Fire_special(int tot_score)
    {
        //Debug.Log(this.gameObject.name + " got fired");
        //bullet_load(this);//만약특수블록이라면,본인load는 하지 않을것

        my_powder.Remove(this);
        //for (int i = 0; i < my_powder.Count; i++)
        //    Debug.Log(my_powder[i].name);
        bullet_send(tot_score);
        dispenser.Explode_candies();
        my_powder.Clear();
        ready_to_bomb = false;
    }

    private void Fire(int tot_score)
    {
        //Debug.Log(this.gameObject.name + " got fired");
        bullet_load(this);//만약특수블록이라면,본인load는 하지 않을것
        bullet_send(tot_score);
        dispenser.Explode_candies();
        my_powder.Clear();
        ready_to_bomb = false;
    }

    private bool Check_Candy_ball()
    {
        Refresh_neighbors();
        bool[] same_hood = new bool[4];

        for (int i = 0; i < 4; i++) {
            same_hood[i] = false;
            if (neighbors[i] == null)
                continue;

            if (!neighbors[i].GetComponent<Candy>().Are_u_same(this_type))
                continue;
            same_hood[i] = true;
            //Debug.Log(gameObject.name + "'s neighbor of " + (dir_4)i + " is same");
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

        for (int i = 0; i < 4; i++)
        {
            same_hood[i] = false;
            if (others_hood[i] == null)
                continue;

            if (!others_hood[i].GetComponent<Candy>().Are_u_same(this_type))
                continue;
            same_hood[i] = true;
            //Debug.Log(gameObject.name + "'s neighbor of " + (dir_4)i + " is same");
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
        return checker;
    }

    public bool Check_all_dir(ref List<Candy> parent_powder)//candyball용 override, Candyball check만을 위함
    {
        //Debug.Log("myname is " + gameObject.name + " and other : " + except_name);
        Refresh_neighbors();

        for (int i = 0; i < 4; i++)
        {
            if (neighbors[i] == null)
                continue;
            if (parent_powder.Contains(neighbors[i]))//stackoverflow 방지
                continue;

            if (neighbors[i].Are_u_same(this_type))
            {
                //Add_candy(ref parent_powder, neighbors[i]);//
                parent_powder.Add(neighbors[i]);
                neighbors[i].Check_all_dir(ref parent_powder);
            }
        }

        return false;
    }

    public bool Check_all_dir(ref List<Candy> row_score, ref List<Candy> col_score)//기본형 override
    {
        Refresh_neighbors();
        for (int i = 0; i < 4; i++)
        {
        if (neighbors[i] == null)
            continue;
        if (row_score.Contains(neighbors[i])|| col_score.Contains(neighbors[i]))//feedback방지
            continue;

        if (!neighbors[i].Are_u_same(this_type))
            continue;

            //my_powder.Add(neighbors[i]);

        if (i > 1)
                Add_candy(ref row_score, neighbors[i]);//row_score.Add(neighbors[i]);
            else
                Add_candy(ref col_score, neighbors[i]);//col_score.Add(neighbors[i]);
            neighbors[i].Check_all_dir(this_type, (dir_4)i, ref row_score, ref col_score);
        }
        
        return false;
    }

    public bool Check_all_dir(Candy[] other_neighbors, ref List<Candy> row_score, ref List<Candy> col_score)//검사용 override
    {
        for (int i = 0; i < 4; i++)
        {
            if (other_neighbors[i] == null)
                continue;

            if (!other_neighbors[i].Are_u_same(this_type))
                continue;

            //Debug.Log(this.gameObject.name + " 'is same with by" + (dir_4)i + " to " + other_neighbors[i].name);
            //parent.Add(other_neighbors[i]);

        if (i > 1)
                Add_candy(ref row_score, other_neighbors[i]);//row_score.Add(other_neighbors[i]);
            else
                Add_candy(ref col_score, other_neighbors[i]);//col_score.Add(other_neighbors[i]);
            other_neighbors[i].Check_all_dir(this_type, (dir_4)i, ref row_score, ref col_score);
        }

        return false;
    }

    public bool Check_all_dir(types check_type, dir_4 prev_dir, ref List<Candy> row_score, ref List<Candy> col_score)//재귀용 override
    {
        int now_ind = (int)prev_dir;

        if (neighbors[now_ind] == null)
            return false;

        if (row_score.Contains(neighbors[now_ind]) || col_score.Contains(neighbors[now_ind]))//feedback방지
            return false;

        if (!neighbors[now_ind].Are_u_same(check_type))
            return false;

        if (now_ind > 1)
            Add_candy(ref row_score, neighbors[now_ind]);//row_score.Add(neighbors[now_ind]);
        else
            Add_candy(ref col_score, neighbors[now_ind]);//col_score.Add(neighbors[now_ind]);

        if (!neighbors[now_ind].Check_all_dir(this_type, prev_dir, ref row_score, ref col_score))
            return false;
        return true;
    }
    //포장지 + 리본 캔디 조합시 range를 조정해주는 방식으로 할것
    private void Add_candy(ref List<Candy> powder, Candy item)//item이 스페셜이면 영역 캔디들 포함시키는 함수
    {
        if (powder.Contains(item))
            return;
        //Add_candy(ref powder, item);
        ////powder.Add(item);
        if ((int)item.addy < 1)
        {
            powder.Add(item);
            return;
        }
        if (item.this_type == types.Ball)//공은 굴려버리기
        {
            dir_4 rand_dir = (dir_4)Random.Range(0, 3);
            item.ball_break(rand_dir);
            Debug.Log("rand dir" + rand_dir);
            return;
        }
        //스페셜일시 조건문
        switch (item.addy)
        {
            case additional.Rx:
                for(int i = 0; i< width; i++)
                {
                    //if (i == item.ind_x)//feedback 방지
                    //    continue;
                    string name_to_break = Name_translate(i, item.ind_y);
                    GameObject temp = GameObject.Find(name_to_break);
                    if (temp == null)
                        continue;
                    Candy temp2 = temp.GetComponent<Candy>();

                    if (powder.Contains(temp2))//stackoverflow 방지
                        continue;
                    //Add_candy(ref powder, temp2);
                    powder.Add(temp2);
                }
                break;

            case additional.Ry:
                for (int j = 0; j < height; j++)
                {
                    //if (j == item.ind_y)//feedback 방지
                    //    continue;

                    string name_to_break = Name_translate(item.ind_x, j);
                    GameObject temp = GameObject.Find(name_to_break);
                    if (temp == null)
                        continue;
                    Candy temp2 = temp.GetComponent<Candy>();

                    if (powder.Contains(temp2))//stackoverflow 방지
                        continue;
                    powder.Add(temp2);
                    //Add_candy(ref powder, temp2);
                }
                break;

            case additional.Sq:

                int bomb_range = 3;
                List<string> names_to_add = new List<string>();
                for (int i = 0; i < bomb_range; i++) {
                    for (int j = 0; j < bomb_range - i; j++)
                    {
                        for (int m = 0; m<2; m++)
                            for (int n = 0; n<2; n++)
                            {
                               string name_to_break = Name_translate(item.ind_x  + (1 - 2*m)*i, item.ind_y + (1 -2*n)*j);
                                if(!names_to_add.Contains(name_to_change))
                                    names_to_add.Add(name_to_break);
                            }
                    }
                }
                //string name_to_break = Name_translate(ind_x + i, ind_y + j);
                //string name_to_break = Name_translate(ind_x + i, ind_y - j);
                //string name_to_break = Name_translate(ind_x - i, ind_y + j);
                //string name_to_break = Name_translate(ind_x - i, ind_y - j);
                Debug.Log("near candies : " + names_to_add.Count);
                for (int i = 0; i < names_to_add.Count; i++)
                {
                    //if (names_to_add[i] == item.name)
                    //    continue;

                    GameObject temp = GameObject.Find(names_to_add[i]);
                    if (temp == null)
                        continue;
                    Candy temp2 = temp.GetComponent<Candy>();

                    if (powder.Contains(temp2))//stackoverflow 방지
                        continue;
                    powder.Add(temp2);
                    //Add_candy(ref powder, temp2);
                }

                break;
            default:break;
        }
    }


    public bool Are_u_same(types candy_type)
    {
        //Debug.Log("my:" + this_type + ", you: " + candy_type);
        if (candy_type != this_type)
            return false;

        return true;

    }


    private void Check_connected()//색이 같은 모든 연결된 캔디 탐색,ball용
    {
        Check_all_dir(ref my_powder);
        Ballize();
        Fire_special(candy_ball_score);
    }

    
    private void Check_score()//기본 검사 함수
    {
        my_powder.Clear();

        Refresh_neighbors();

        List<Candy> row_powder = new List<Candy>();
        List<Candy> col_powder = new List<Candy>();

        if (!Check_Candy_ball())
            Check_all_dir(ref row_powder, ref col_powder);
        else
            return;
        //Debug.Log(gameObject.name + "'s row_score:" + row_score + ", col_score: " + col_score);// +  "i+1 : " + (dir_4)(1 + 2));

        if(Calc_score(row_powder, col_powder))
        {
            //내가 스페셜이면 영역 캔디들 포함시키는 함수 입력
            //Add_candy(my_powder, neighbors[i]);
            int max_score = (int)Mathf.Max(row_powder.Count, col_powder.Count);
            if (Check_additinal(row_powder, col_powder))
                Fire_special(max_score);
            else
                Fire(max_score);
        }
    }

        private bool Check_score(Candy[] other_neighbors, Candy other)//swipe 검사하기 위해 override
    {
        my_powder.Clear();

        List<Candy> row_powder = new List<Candy>();
        List<Candy> col_powder = new List<Candy>();

        if (!Check_Candy_ball(other_neighbors,  other))
            Check_all_dir(other_neighbors, ref row_powder, ref col_powder);
        else
            return true;


        //Debug.Log(gameObject.name + "'s row_score:" + row_score + ", col_score: " + col_score);// +  "i+1 : " + (dir_4)(1 + 2));


        if(Calc_score(row_powder, col_powder))
            return true;


        return false;

    }

    private bool Calc_score(List<Candy> row_powder, List<Candy> col_powder)//검사가 유효한지 검사
    {
        if (row_powder.Count > 1)
            my_powder.AddRange(row_powder);
        if (col_powder.Count > 1)
            my_powder.AddRange(col_powder);

        return my_powder.Count > 1 ;
    }


    public void Bomb()
    {
        dispenser.Refill_plz(this);
        //Debug.Log("candy " + gameObject.name + "requested refill");
        Destroy(this.gameObject);
    }


    public void Refresh_neighbors()
    {

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
            Refresh_neighbors();
        if (is_roll)
            ball_move();
        if (is_wait)
            visible_by_time();
    }
}
