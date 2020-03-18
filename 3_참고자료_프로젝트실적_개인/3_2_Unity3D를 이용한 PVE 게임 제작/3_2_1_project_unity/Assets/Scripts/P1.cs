using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class P1 : MonoBehaviour {

    public float maxenrg = 1.000f;
    public float increase = 0.300f;
    public float decrease = 0.100f;
    public float jumpheight = 10.0f;
    public float dashmount = 5.0f;
    private static bool isjump, tspace, tup, tdown, tleft, tright = false;
    private float renrg,lenrg,uenrg = 0.000f;
    private bool isGrounded,isstunned,isdash = false;
    // Use this for initialization
    void Start()
    {

    }

    private void OnCollisionEnter(Collision col)

    {

        if (col.gameObject.tag == "Ground")

            isGrounded = true;
    }

    private void OnCollisionStay(Collision col)
    {
        if (col.gameObject.tag == "Ground")

            isGrounded = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyUp(KeyCode.Space))
            tspace = false;
        if (Input.GetKeyUp(KeyCode.UpArrow))
            tup = false;
        if (Input.GetKeyUp(KeyCode.DownArrow))
            tdown = false;
        if (Input.GetKeyUp(KeyCode.LeftArrow))
            tleft = false;
        if (Input.GetKeyUp(KeyCode.RightArrow))
            tright = false;

        bool isNatk = GameObject.Find("knight").GetComponent<P1_anim>().IsNonAtk();
        if (GameObject.Find("knight").GetComponent<P1_anim>().IsStunned())
        {
            renrg = 0.000f;
            uenrg = 0.000f;
            lenrg = 0.000f;
            //Debug.Log("returned");
            return;
        }
        // planet.transform.position;

        if (Input.GetKey(KeyCode.Space))
            tspace = true;

        if (Input.GetKey(KeyCode.UpArrow))
            tup = true;

        if (Input.GetKey(KeyCode.DownArrow))
            tdown = true;

        if (Input.GetKey(KeyCode.LeftArrow))
            tleft = true;

        if (Input.GetKey(KeyCode.RightArrow))
            tright = true;




        // 0,1,0 = up
        // 1,0,0 = right
        //if (tspace)
        //    transform.Translate(Vector3.up * fwdspeed * Time.deltaTime, Space.Self);

        if (isGrounded && tspace && uenrg < maxenrg)
        {

            uenrg += jumpheight;
            isGrounded = false;
            isdash = true;
        }

        //if (tdown && denrg < maxenrg)
        //    transform.Rotate(Vector3.right, -speed, Space.Self);

        if (tleft && lenrg < maxenrg && isNatk)
            lenrg += increase;
       // Debug.Log(tright);
        if (tright && renrg < maxenrg && isNatk)
            renrg += increase;
        
        //dash
        if (isdash&&!isGrounded && uenrg < 1 && tleft && Input.GetKeyDown(KeyCode.Space) && isNatk)
        {
            lenrg += dashmount;
            renrg = 0;
            isdash = false;
        }
        //Debug.Log(uenrg);

        if (isdash&&!isGrounded && uenrg < 1 && tright && Input.GetKeyDown(KeyCode.Space)&& isNatk)
        {
            renrg += dashmount;
            lenrg = 0;
            isdash = false;
            //Debug.Log(isGrounded);

        }

        

        if (lenrg > 0)
        {
           // if(isNatk)
            transform.Translate(Vector3.left * lenrg * Time.deltaTime, Space.Self);
            lenrg -= decrease;
        }
        if (renrg > 0)
        {
           // if(isNatk)
            transform.Translate(Vector3.right * renrg * Time.deltaTime, Space.Self);
            renrg -= decrease;
        }

        if (uenrg > 0)
        {
            transform.Translate(Vector3.up * uenrg * Time.deltaTime, Space.Self);
            uenrg -= decrease*10;
        }
    }
    public bool isGround()
    {
        return isGrounded;
    }
    public bool isDashing()
    {
        return !isdash;
    }
}
