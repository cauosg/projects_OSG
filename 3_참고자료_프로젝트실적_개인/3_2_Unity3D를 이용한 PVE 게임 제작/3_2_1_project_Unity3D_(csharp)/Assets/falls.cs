using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class falls : MonoBehaviour {

    public GameObject Dusts;
    public GameObject Rings;
    public float orgheight = 20.0f;
    public float cooldown = 5.0f;
    public float spd = 1.0f;
    private float cooltime = 0.0f;
    private bool ischeck = true;

    private bool isact = true;

    void Start()
    {
        Dusts.transform.Translate(Vector3.up * orgheight, Space.World);
        cooltime = cooldown;
        
    }

    // Update is called once per frame
    void Update()
    {
        if (!isact)
        {
            //gameObject.SetActive(false);
            return;
        }
        if (cooltime < 0.1f)
        {
            //Dusts.transform.Translate(Vector3.up * orgheight, Space.World);
            float randx = -30.0f + Random.Range(0, 6.0f);
            Dusts.transform.position = new Vector3(randx,orgheight,-9.07f);
            Rings.transform.position = new Vector3(randx, -0.4f, -9.07f);
            cooltime = cooldown;
            Rings.SetActive(true);
            ischeck = true;
        }
        Dusts.transform.Translate(Vector3.down * spd*(cooldown - cooltime) * (cooldown - cooltime) * Time.deltaTime, Space.World);
        cooltime -= Time.deltaTime;
       // Debug.Log(cooldown - cooltime);
        if (Dusts.transform.position.y < 50)
        {

            Rings.SetActive(false);
            ischeck = false;
        }
    }
    public void Setact(bool k)
    {
        isact = k;
        //if (isact)
        //    gameObject.SetActive(true);
        //else
        //    gameObject.SetActive(false);
    }
}
