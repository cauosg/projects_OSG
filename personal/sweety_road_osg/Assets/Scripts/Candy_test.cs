using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Candy_test : MonoBehaviour
{

    public GameObject[] Candies;

    private GameObject its_Candy;
    private int this_type;
    private bool is_lerp, update_neighbors, is_show = false;
    private float spd = 0.000f;

    public int queues = 7;
    public float accel_factor = 0.00f;//1000으로 나눠줄 예정
    public float max_spd = 0.2f;
    public float zorder = -0.2f;
    Vector2 now_pos, dest_pos;
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
    public void Init(int candy_type, Vector2 begin_pos, Vector2 target_pos, float interval)
    {
        //Debug.Log(candy_type);
        //Debug.Log(Candies[candy_type]);

        accel_factor *= 0.001f;
        this.this_type = candy_type - 2;//0과 1인거 제외하기위해 보정

        its_Candy = Instantiate(Candies[this_type], now_pos, Quaternion.identity) as GameObject;
        float scale_factor = interval / its_Candy.GetComponent<SpriteRenderer>().bounds.size.x;
        its_Candy.transform.localScale = new Vector3(scale_factor, scale_factor, scale_factor);
        its_Candy.transform.SetParent(this.transform);
        its_Candy.transform.Translate(0, 0, zorder);
        its_Candy.GetComponent<SpriteRenderer>().enabled = false;

        this.now_pos = begin_pos;
        //this.transform.position = begin_pos
        this.dest_pos = target_pos;
        this.transform.position = dest_pos + new Vector2(0, queues * interval);
        this.is_lerp = true;
        //Debug.Log(now_pos);
        //Debug.Log(dest_pos);


    }

    // Use this for initialization
    void Start()
    {

    }

    private void Find_neighbors()
    {

    }

    //private void Init_lerp()
    //   {
    //       spd = 0.000f;
    //   }

    private void End_lerp()
    {

        spd = 0.000f;
        is_lerp = false;
        now_pos = dest_pos;
        this.transform.position = dest_pos;
    }
    private void Lerp_move()
    {
        //Debug.Log("im moving now....");
        spd += accel_factor;
        if (spd > max_spd)
            spd = max_spd;

        this.transform.Translate(new Vector2(0, -spd));
        //Debug.Log(this.transform.position);
        if (this.transform.position.y <= dest_pos.y + 0.1f)
            End_lerp();
        if (!is_show && this.transform.position.y < now_pos.y)
            its_Candy.GetComponent<SpriteRenderer>().enabled = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (is_lerp)
            Lerp_move();
    }
}
